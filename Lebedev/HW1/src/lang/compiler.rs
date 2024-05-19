use std::collections::HashMap;

use crate::interpreter::{Address, ALUInstructionKind, Instruction, JumpInstructionKind, MemoryInstructionKind, RawInstruction, Reference, Register, Word};
use crate::lang::parser::{Expr, Identifier, Stmt};

pub fn compile(body: Vec<Stmt>) -> CompiledProgram {
    let mut ctx = Context::default();
    compile_body(&mut ctx, &body);
    CompiledProgram::new(ctx.instructions, ctx.words)
}

pub struct CompiledProgram {
    instructions: Vec<Instruction>,
    constants: Vec<Word>,
}

impl CompiledProgram {
    pub fn new(instructions: Vec<Instruction>, constants: Vec<Word>) -> CompiledProgram {
        CompiledProgram { instructions, constants }
    }

    pub fn crete_buffer(&self) -> Vec<u8> {
        let mut storage = vec![];
        self.write_instructions(&mut storage);
        self.write_terminating_instruction(&mut storage);
        self.write_constants(&mut storage);
        storage
    }

    fn write_instructions(&self, storage: &mut Vec<u8>) {
        for instruction in &self.instructions {
            let buffer = instruction.create_buffer();
            storage.append(&mut Vec::from(buffer));
        }
    }

    fn write_constants(&self, storage: &mut Vec<u8>) {
        for constant in self.constants.iter().rev() {
            let buffer = constant.create_buffer();
            storage.append(&mut Vec::from(buffer));
        }
    }

    fn write_terminating_instruction(&self, storage: &mut Vec<u8>) {
        storage.append(&mut vec![0; 8]);
    }
}

#[derive(Default)]
struct Context {
    instructions: Vec<Instruction>,
    words: Vec<Word>,
    identifiers: HashMap<Identifier, usize>,
}

impl Context {
    const ZERO: Register = Register::new(0);
    const RES: Register = Register::new(10);

    const R1: Register = Register::new(11);
    const R2: Register = Register::new(12);
    const R3: Register = Register::new(13);
    const R4: Register = Register::new(14);
    const R5: Register = Register::new(15);

    fn new_const(&mut self, word: Word) -> i32 {
        self.words.push(word);
        (self.words.len() - 1) as i32
    }

    fn identifier(&mut self, identifier: Identifier) -> Reference {
        if let Some(idx) = self.identifiers.get(&identifier) {
            return Reference::StackReference(*idx);
        }
        let len = self.identifiers.len();
        self.identifiers.insert(identifier, len);
        Reference::StackReference(len)
    }

    fn index_to_address(index: usize) -> Address {
        Address::new((index * RawInstruction::SIZE) as i32)
    }
}

fn compile_body(ctx: &mut Context, body: &Vec<Stmt>) {
    for stmt in body {
        match stmt {
            Stmt::Print(expr) => {
                compile_expr(ctx, expr);
                ctx.instructions.push(Instruction::ALUInstruction {
                    kind: ALUInstructionKind::PRINT,
                    lhs: Context::RES,
                    rhs: Context::ZERO,
                    res: Context::RES,
                });
            }
            Stmt::Assign(identifier, expr) => {
                compile_expr(ctx, expr);
                let instruction = Instruction::MemoryInstruction {
                    kind: MemoryInstructionKind::Store,
                    reg: Context::RES,
                    reference: ctx.identifier(identifier.clone()),
                };
                ctx.instructions.push(instruction);
            }
            Stmt::If(ref expr, ref stmts) => {
                compile_expr(ctx, expr);
                ctx.instructions.push(Instruction::JumpInstruction {
                    kind: JumpInstructionKind::CJUMP,
                    reg: Context::RES,
                    addr: Address::DUMMY,
                });
                let jump_index = ctx.instructions.len() - 1;
                compile_body(ctx, stmts);
                let next_instruction_index = ctx.instructions.len();
                let jump_instruction = &mut ctx.instructions[jump_index];
                if let Instruction::JumpInstruction { addr, .. } = jump_instruction {
                    *addr = Context::index_to_address(next_instruction_index);
                } else {
                    panic!("bug!")
                }
            }
            Stmt::While(expr, stmts) => {
                let expr_address = Context::index_to_address(ctx.instructions.len());
                compile_expr(ctx, expr);
                let expr_jump_index = ctx.instructions.len();
                ctx.instructions.push(Instruction::JumpInstruction {
                    kind: JumpInstructionKind::CJUMP,
                    reg: Context::RES,
                    addr: Address::DUMMY
                });
                compile_body(ctx, stmts);
                let loop_jump_instruction = Instruction::JumpInstruction {
                    kind: JumpInstructionKind::JUMP,
                    reg: Context::ZERO,
                    addr: expr_address,
                };
                ctx.instructions.push(loop_jump_instruction);
                let next_instruction_index = ctx.instructions.len();
                let cond_jump_instruction = &mut ctx.instructions[expr_jump_index];
                if let Instruction::JumpInstruction { addr, .. } = cond_jump_instruction {
                    *addr = Context::index_to_address(next_instruction_index);
                } else {
                    panic!("bug!")
                }
            }
            Stmt::VarDecl(_) => {}
        }
    }
}

fn compile_expr(ctx: &mut Context, expr: &Expr) {
    match expr {
        Expr::Literal(num) => {
            let constant = ctx.new_const(Word::new(*num)) * (std::mem::size_of::<Word>() as i32);
            ctx.instructions.push(Instruction::MemoryInstruction {
                kind: MemoryInstructionKind::LoadConst,
                reg: Context::RES,
                reference: Reference::AddressReference(Address::new(constant)),
            });
        }
        Expr::Sum(box (lhs, rhs)) => compile_binop(ctx, lhs, rhs, ALUInstructionKind::ADD),
        Expr::Sub(box (lhs, rhs)) => compile_binop(ctx, lhs, rhs, ALUInstructionKind::SUB),
        Expr::And(box (lhs, rhs)) => compile_binop(ctx, lhs, rhs, ALUInstructionKind::AND),
        Expr::Or(box (lhs, rhs)) => compile_binop(ctx, lhs, rhs, ALUInstructionKind::OR),
        Expr::Not(box expr) => {
            compile_expr(ctx, expr);
            ctx.instructions.push(Instruction::ALUInstruction {
                kind: ALUInstructionKind::NOT,
                res: Context::RES,
                lhs: Context::RES,
                rhs: Context::ZERO,
            });
        }
        Expr::VarRef(ident) => {
            let reference = ctx.identifier(ident.clone());
            ctx.instructions.push(Instruction::MemoryInstruction {
                kind: MemoryInstructionKind::LoadStack,
                reg: Context::RES,
                reference,
            })
        }
        Expr::LT(box (lhs, rhs)) => compile_binop(ctx, lhs, rhs, ALUInstructionKind::LT),
        Expr::GT(box (lhs, rhs)) => compile_binop(ctx, rhs, lhs, ALUInstructionKind::LT),
        Expr::LE(box (lhs, rhs)) => compile_le(ctx, lhs, rhs),
        Expr::GE(box (lhs, rhs)) => compile_le(ctx, rhs, lhs),
        Expr::Eq(box (lhs, rhs)) => compile_binop(ctx, lhs, rhs, ALUInstructionKind::EQ),
    }
}

fn compile_le(ctx: &mut Context, lhs: &Expr, rhs: &Expr) {
    compile_binop(ctx, lhs, rhs, ALUInstructionKind::LT);
    ctx.instructions.push(Instruction::ALUInstruction {
        kind: ALUInstructionKind::EQ,
        lhs: Context::R1,
        rhs: Context::R2,
        res: Context::R3,
    });
    ctx.instructions.push(Instruction::ALUInstruction {
        kind: ALUInstructionKind::OR,
        lhs: Context::RES,
        rhs: Context::R3,
        res: Context::RES
    })
}

// Note: compile_le uses this implementation as contract
fn compile_binop(ctx: &mut Context, lhs: &Expr, rhs: &Expr, alu_kind: ALUInstructionKind) {
    compile_expr(ctx, lhs);
    ctx.instructions.push(Instruction::ALUInstruction {
        kind: ALUInstructionKind::ADD,
        lhs: Context::RES,
        rhs: Context::ZERO,
        res: Context::R1,
    });
    compile_expr(ctx, rhs);
    ctx.instructions.push(Instruction::ALUInstruction {
        kind: ALUInstructionKind::ADD,
        lhs: Context::RES,
        rhs: Context::ZERO,
        res: Context::R2,
    });
    ctx.instructions.push(Instruction::ALUInstruction {
        kind: alu_kind,
        lhs: Context::R1,
        rhs: Context::R2,
        res: Context::RES,
    })
}
