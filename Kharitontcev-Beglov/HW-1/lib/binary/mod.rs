pub mod instructions;

use std::collections::{HashMap, LinkedList};
use crate::binary::instructions::{Instr, IType, JType, RType, transform_to_bytes};
use crate::parser::ast::{Ident, Ops};
use crate::stack_machine::sm::{Condition, Label, StackCommand};


#[derive(Clone, Debug)]
pub struct SMTransformer {
    // The constants map maps all possible `const` instructions into the offset in the data array
    constants: HashMap<i32, usize>,
    constants_order: Vec<i32>,
    next_free_constant_offset: usize,

    // The variables map maps all variable identifiers to the memory address, since there is no scopes
    variables: HashMap<Ident, usize>,
    next_free_variable_offset: usize,

    // The labels map maps all labels to the index of the next instructions
    labels: HashMap<Label, usize>,
}

impl SMTransformer {
    const SP: u8 = 29;
    const OPERAND_1: u8 = 8;
    const OPERAND_2: u8 = 9;
    const VARIABLE_LOAD_TMP: u8 = 10;
    const STACK_INCREMENT: u8 = 26;
    const ZERO: u8 = 0; // Zero register

    pub fn new() -> Self {
        let mut res = Self {
            constants: HashMap::new(),
            constants_order: Vec::new(),
            variables: HashMap::new(),
            labels: HashMap::new(),
            next_free_variable_offset: 0,
            next_free_constant_offset: 0,
        };
        res.push_constant(&1);
        res
    }

    fn push_constant(&mut self, constant: &i32) {
        if self.constants.contains_key(constant) {
            return;
        }
        self.constants.insert(*constant, self.next_free_constant_offset);
        self.constants_order.push(*constant);
        self.next_free_constant_offset += 1
    }

    fn push_identifier(&mut self, ident: &Ident) {
        if self.variables.contains_key(ident) {
            return;
        }
        self.variables.insert(ident.clone(), self.next_free_variable_offset);
        self.next_free_variable_offset += 1
    }

    fn push_label(&mut self, label: &Label, id: usize) {
        if self.labels.contains_key(label) {
            println!("Overwriting label {}", label)
        }
        self.labels.insert(*label, id);
    }

    fn collect_constants(&mut self, program: &Vec<StackCommand>) {
        for instr in program {
            match instr {
                StackCommand::Const(x) => self.push_constant(x),
                _ => {}
            }
        }
    }

    fn collect_identifiers(&mut self, program: &Vec<StackCommand>) {
        for instr in program {
            match instr {
                StackCommand::Load(id) => self.push_identifier(id),
                StackCommand::Store(id) => self.push_identifier(id),
                _ => {}
            }
        }
    }

    fn collect_labels(&mut self, program: &Vec<StackCommand>) {
        let mut meaningful_instructions: usize = 0;
        for instr in program {
            match instr {
                StackCommand::Label(l) => self.push_label(l, meaningful_instructions),
                _ => meaningful_instructions += 1
            }
        }
    }

    fn pop_from_stack_into(&self, reg: u8) -> LinkedList<u8> {
        let load = Instr::I(IType::Lw {
            rs: Self::SP,
            rt: reg,
            imm: 0,
        });
        let one_index = *self.constants.get(&1).unwrap() as u16;
        let load_one = Instr::I(IType::Lw {
            rs: Self::ZERO,
            rt: Self::STACK_INCREMENT,
            imm: one_index << 2,
        });
        let sub = Instr::R(RType {
            rs: Self::SP,
            rt: Self::STACK_INCREMENT,
            rd: Self::SP,
            funct: 34,
        });
        let order = [load, load_one, sub];
        let mut result = LinkedList::new();
        for inst in order {
            result.append(&mut LinkedList::from(transform_to_bytes(&inst).to_be_bytes()));
        }
        result
    }
    fn push_into_stack(&self, reg: u8) -> LinkedList<u8> {
        let save = Instr::I(IType::Sw {
            rs: Self::SP,
            rt: reg,
            imm: 0,
        });
        let one_index = *self.constants.get(&1).unwrap() as u16;
        let load_one = Instr::I(IType::Lw {
            rs: Self::ZERO,
            rt: Self::STACK_INCREMENT,
            imm: one_index << 2,
        });
        let add = Instr::R(RType {
            rs: Self::SP,
            rt: Self::STACK_INCREMENT,
            rd: Self::SP,
            funct: 32,
        });
        let order = [save, load_one, add];
        let mut result = LinkedList::new();
        for inst in order {
            result.append(&mut LinkedList::from(transform_to_bytes(&inst).to_be_bytes()));
        }
        result
    }

    fn load_const_to(&self, reg: u8, x: i32) -> LinkedList<u8> {
        let index = *self.constants.get(&x).unwrap() as u16;
        let load = Instr::I(IType::Lw {
            rs: Self::ZERO,
            rt: reg,
            imm: index << 2,
        });
        LinkedList::from(transform_to_bytes(&load).to_be_bytes())
    }
    fn load_variable_to(&self, reg: u8, ident: &Ident) -> LinkedList<u8> {
        let index = *self.variables.get(ident).unwrap() as u16;
        let load = Instr::I(IType::Lw {
            rs: Self::VARIABLE_LOAD_TMP,
            rt: reg,
            imm: index << 2,
        });
        LinkedList::from(transform_to_bytes(&load).to_be_bytes())
    }

    fn save_variable_from(&self, reg: u8, ident: &Ident) -> LinkedList<u8> {
        let index = *self.variables.get(ident).unwrap() as u16;
        let save = Instr::I(IType::Sw {
            rs: Self::VARIABLE_LOAD_TMP,
            rt: reg,
            imm: index << 2,
        });
        LinkedList::from(transform_to_bytes(&save).to_be_bytes())
    }
    fn get_r_type_operation(op: &Ops, rs: u8, rt: u8, rd: u8) -> LinkedList<u8> {
        let funct = match op {
            Ops::Add => 32u8,
            Ops::Sub => 34u8,
            Ops::BitwiseAnd => 36u8,
            Ops::BitwiseOr => 37u8,
            Ops::BitwiseNor => 39u8,
        };
        let op = Instr::R(RType {
            rs,
            rt,
            rd,
            funct,
        });
        LinkedList::from(transform_to_bytes(&op).to_be_bytes())
    }
    
    fn get_label_address(&self, l: &Label) -> u32 {
        let next_instruction_index = *self.labels.get(l).unwrap() as u32;
        next_instruction_index << 2
    }


    fn transform_instruction(&self, instruction: &StackCommand) -> LinkedList<u8> {
        match instruction {
            StackCommand::Print => {
                let mut result = self.pop_from_stack_into(Self::OPERAND_1);
                result.append(&mut LinkedList::from(transform_to_bytes(
                    &Instr::R(RType{
                        rs: Self::OPERAND_1,
                        rt: 0,
                        rd: 0,
                        funct: 0
                    })
                ).to_be_bytes()));
                result
            }
            StackCommand::Op(op) => {
                let load_1 = self.pop_from_stack_into(Self::OPERAND_1);
                let load_2 = self.pop_from_stack_into(Self::OPERAND_2);
                let op = Self::get_r_type_operation(
                    op,
                    Self::OPERAND_1,
                    Self::OPERAND_2,
                    Self::OPERAND_1,
                );
                let push = self.push_into_stack(Self::OPERAND_1);
                let order = [load_1, load_2, op, push];
                let mut result = LinkedList::new();
                for mut cmd in order {
                    result.append(&mut cmd);
                }
                result
            }
            StackCommand::Load(id) => {
                let mut result = self.load_variable_to(Self::OPERAND_1, id);
                let mut push = self.push_into_stack(Self::OPERAND_1);
                result.append(&mut push);
                result
            }
            StackCommand::Store(id) => {
                let mut pop = self.pop_from_stack_into(Self::OPERAND_1);
                let mut save = self.save_variable_from(Self::OPERAND_1, id);
                pop.append(&mut save);
                pop
            }
            StackCommand::Const(num) => {
                let mut result = self.load_const_to(Self::OPERAND_1, *num);
                let mut push = self.push_into_stack(Self::OPERAND_1);
                result.append(&mut push);
                result
            }
            StackCommand::Label(_) => LinkedList::new(),
            StackCommand::Jmp(l) => {
                LinkedList::from(transform_to_bytes(&Instr::J(JType::Jmp {
                    address: self.get_label_address(l)
                })).to_be_bytes())
            }
            StackCommand::ConditionalJump(Condition::EqualsZero, l) => {
                // if equals, then jump
                let pop = self.pop_from_stack_into(Self::OPERAND_1);
                let branch = LinkedList::from(transform_to_bytes(&Instr::I(IType::Bne {
                    rs: Self::OPERAND_1,
                    rt: Self::ZERO,
                    imm: 0
                })).to_be_bytes());
                let jump = LinkedList::from(transform_to_bytes(&Instr::J(JType::Jmp {
                    address: self.get_label_address(l)
                })).to_be_bytes());
                let mut result=  LinkedList::new();
                let order = [pop, branch, jump];
                for mut instr in order {
                    result.append(&mut instr);
                }
                result
            }
            StackCommand::ConditionalJump(Condition::NotEqualsZero, l) => {
                // if not equals, then jump
                let pop = self.pop_from_stack_into(Self::OPERAND_1);
                let branch = LinkedList::from(transform_to_bytes(&Instr::I(IType::Beq {
                    rs: Self::OPERAND_1,
                    rt: Self::ZERO,
                    imm: 0
                })).to_be_bytes());
                let jump = LinkedList::from(transform_to_bytes(&Instr::J(JType::Jmp {
                    address: self.get_label_address(l)
                })).to_be_bytes());
                let mut result=  LinkedList::new();
                let order = [pop, branch, jump];
                for mut instr in order {
                    result.append(&mut instr);
                }
                result
            }
        }
    }
    
    fn get_loader_code(&self) -> LinkedList<u8> {
        self.load_const_to(Self::VARIABLE_LOAD_TMP, self.constants_order.len() as i32)
    }
    
    pub fn transform_program(&mut self, program: &Vec<StackCommand>) -> (Vec<u8>, Vec<u8>) {
        self.collect_constants(program);
        self.push_constant(&(self.constants_order.len() as i32 + 1));
        self.collect_identifiers(program);
        self.collect_labels(program);
        let mut constants_result = LinkedList::new();
        for constant in &self.constants_order {
            constants_result.append(&mut LinkedList::from(constant.to_be_bytes()))
        }
        let mut code_result = self.get_loader_code();
        for instr in program {
            code_result.append(&mut self.transform_instruction(instr))
        }
        (constants_result.into_iter().collect(), code_result.into_iter().collect())
    }
}

