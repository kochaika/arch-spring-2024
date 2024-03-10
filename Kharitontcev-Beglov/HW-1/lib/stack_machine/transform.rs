use std::collections::{LinkedList};
use crate::parser::ast::{Block, Expr, Ops, PrefixOps, Stmt};
use crate::stack_machine::sm::{Condition, Label, StackCommand};

pub struct AstTransformer {
    last_id: i32,
}

impl AstTransformer {
    pub fn new() -> Self {
        AstTransformer { last_id: 0 }
    }

    fn generate_label(&mut self) -> Label {
        let label = Label { id: self.last_id };
        self.last_id += 1;
        label
    }
    fn transform_expr_to_sm(&self, expr: &Expr) -> LinkedList<StackCommand> {
        match expr {
            Expr::IntLiteral(n) => LinkedList::from([StackCommand::Const(n.clone())]),
            Expr::Var(x) => LinkedList::from([StackCommand::Load(x.clone())]),
            Expr::InfixOperation(left, op, right) => {
                let mut left = self.transform_expr_to_sm(left.as_ref());
                left.append(&mut self.transform_expr_to_sm(right.as_ref()));
                left.push_back(StackCommand::Op(*op));
                left
            }
            Expr::PrefixOperation(ops, expr) => {
                match ops {
                    PrefixOps::BitwiseNot => {
                        let mut left = self.transform_expr_to_sm(expr.as_ref());
                        left.push_back(StackCommand::Const(0));
                        left.push_back(StackCommand::Op(Ops::BitwiseNor));
                        left
                    },
                    PrefixOps::UnaryMinus => {
                        let mut left = self.transform_expr_to_sm(expr.as_ref());
                        left.push_back(StackCommand::Const(0));
                        left.push_back(StackCommand::Op(Ops::Sub));
                        left
                    }
                }
            }
        }
    }

    fn transform_stmt(&mut self, label: Label, stmt: &Stmt) -> (bool, LinkedList<StackCommand>) {
        match stmt {
            Stmt::VarDeclaration(_) => (false, LinkedList::new()),
            Stmt::VarAssign(id, expr) => {
                let mut expr_cmds = self.transform_expr_to_sm(expr);
                expr_cmds.push_back(StackCommand::Store(id.clone()));
                (false, expr_cmds)
            }
            Stmt::If { condition, true_branch, false_branch } => {
                let mut condition_code = self.transform_expr_to_sm(condition);
                let false_label = self.generate_label();
                condition_code.push_back(StackCommand::ConditionalJump(Condition::EqualsZero, false_label));
                let (_, mut true_code) = self.transform_block_to_sm(label, true_branch.as_ref());
                condition_code.append(&mut true_code);
                condition_code.push_back(StackCommand::Jmp(label));
                condition_code.push_back(StackCommand::Label(false_label));
                if let Some(false_b) = false_branch {
                    condition_code.append(&mut self.transform_block_to_sm(label, false_b.as_ref()).1);
                }
                (true, condition_code)
            }
            Stmt::While(condition, body) => {
                let start_body_label = self.generate_label();
                let after_body_label = self.generate_label();
                let mut result = LinkedList::from([
                    StackCommand::Jmp(after_body_label),
                    StackCommand::Label(start_body_label)
                ]);
                let (_, mut body_code) = self.transform_block_to_sm(after_body_label, body);
                let mut condition_code = self.transform_expr_to_sm(condition);
                result.append(&mut body_code);
                result.push_back(StackCommand::Label(after_body_label));
                result.append(&mut condition_code);
                result.push_back(StackCommand::ConditionalJump(Condition::NotEqualsZero, start_body_label));
                (false, result)
            }
            Stmt::Print(expr) => {
                let mut result = self.transform_expr_to_sm(expr);
                result.push_back(StackCommand::Print);
                (false, result)
            }
        }
    }

    fn transform_block_to_sm(&mut self, label: Label, program: &Block) -> (bool, LinkedList<StackCommand>) {
        if program.is_empty() {
            return (false, LinkedList::new())
        }
        let mut result = LinkedList::new();
        for i in 0..(program.len() - 1) {
            let after_stmt = self.generate_label();
            let (lab_used, mut code) = self.transform_stmt(after_stmt, &program[i]);
            result.append(&mut code);
            if lab_used {
                result.push_back(StackCommand::Label(after_stmt));
            }
        }
        let (used, mut code) =
            self.transform_stmt(label, &program[program.len() - 1]);
        result.append(&mut code);
        (used, result)
    }

    pub fn transform_ast_to_sm(&mut self, program: Block) -> Vec<StackCommand> {
        let after_stmt = self.generate_label();
        let (lab_used, mut code) = self.transform_block_to_sm(after_stmt, &program);
        if lab_used {
            code.push_back(StackCommand::Label(after_stmt));
        }
        code.into_iter().collect()
    }
}