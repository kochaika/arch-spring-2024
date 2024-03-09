use std::fmt::{Display, Formatter};
use crate::parser::ast::{Ident, Ops};

#[derive(Clone, Debug)]
pub enum StackCommand {
    Print,
    Op(Ops),
    Load(Ident),
    Store(Ident),
    Const(i32),
    Label(Label),
    Jmp(Label),
    ConditionalJump(Condition, Label),
}
#[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
pub struct Label {
    pub id: i32,
}

impl Display for Label {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "LABEL {}:", self.id)
    }
}

#[derive(Copy, Clone, Debug)]
pub enum Condition {
    EqualsZero,
    NotEqualsZero,
}