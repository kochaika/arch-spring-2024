#[derive(Eq, PartialEq, Clone, Debug, Hash)]
pub struct Ident(pub String);

pub type Block = Vec<Stmt>;

#[derive(PartialEq, Clone, Debug)]
pub enum Stmt {
    VarDeclaration(Vec<Ident>),
    VarAssign(Ident, Expr),
    If { condition: Expr, true_branch: Box<Block>, false_branch: Option<Box<Block>> },
    While(Expr, Box<Block>),
    Print(Expr),
}

#[derive(PartialEq, Clone, Debug)]
pub enum Expr {
    IntLiteral(i32),
    Var(Ident),
    InfixOperation(Box<Expr>, Ops, Box<Expr>),
    PrefixOperation(PrefixOps, Box<Expr>),
}

#[derive(PartialEq, Eq, Ord, PartialOrd, Copy, Clone, Debug)]
pub enum Ops {
    Add,
    Sub,
    BitwiseAnd,
    BitwiseOr,
    BitwiseNor
}

#[derive(PartialEq, Clone, Debug, Ord, PartialOrd, Eq)]
pub enum PrefixOps {
    BitwiseNot,
}

#[derive(PartialEq, PartialOrd, Debug, Clone)]
pub enum Precedence {
    Lowest,
    BitOperations,
    Operations,
}