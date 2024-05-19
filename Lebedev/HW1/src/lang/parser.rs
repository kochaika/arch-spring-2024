use chumsky::prelude::*;
use chumsky::text::{ident, int};

#[derive(Debug)]
pub enum Stmt {
    Print(Expr),
    VarDecl(Identifier),
    Assign(Identifier, Expr),
    If(Expr, Vec<Stmt>),
    While(Expr, Vec<Stmt>)
}

#[derive(Debug, Eq, PartialEq, Hash, Clone)]
pub struct Identifier(String);

#[derive(Debug)]
pub enum Expr {
    Literal(i32),
    Sum(Box<(Expr, Expr)>),
    Sub(Box<(Expr, Expr)>),
    And(Box<(Expr, Expr)>),
    Or(Box<(Expr, Expr)>),
    LT(Box<(Expr, Expr)>),
    LE(Box<(Expr, Expr)>),
    GT(Box<(Expr, Expr)>),
    GE(Box<(Expr, Expr)>),
    Eq(Box<(Expr, Expr)>),
    Not(Box<Expr>),
    VarRef(Identifier),
}

#[derive(Clone, Eq, PartialEq, Hash, Debug)]
enum Token {
    Literal(i32), Identifier(String),
    Plus, Minus, And, Or, Not, Eq,
    Semicolon,
    While, If, Let, Print,
    LT, LE, GT, GE, EqEq,
    LBrace, RBrace, LParen, RParen,
}

fn lexer<'src>() -> impl Parser<char, Vec<Token>, Error=Simple<char>> {
    let num = int(10).map(|num: String| Token::Literal(num.parse::<i32>().unwrap()));
    let plus = just('+').map(|_| Token::Plus);
    let minus = just('-').map(|_| Token::Minus);
    let and = just("&").map(|_| Token::And);
    let or = just("|").map(|_| Token::Or);
    let lt = just("<").map(|_| Token::LT);
    let le = just("<=").map(|_| Token::LE);
    let gt = just(">").map(|_| Token::GT);
    let ge = just(">=").map(|_| Token::GE);
    let eq_eq = just("==").map(|_| Token::EqEq);
    let not = just('!').map(|_| Token::Not);
    let eq = just('=').map(|_| Token::Eq);
    let semicolon = just(";").map(|_| Token::Semicolon);
    let while_kw = just("while").map(|_| Token::While);
    let if_kw = just("if").map(|_| Token::If);
    let let_kw = just("let").map(|_| Token::Let);
    let print_kw = just("print").map(|_| Token::Print);
    let identifier = ident().map(|identifier: String| Token::Identifier(identifier));
    let lbrace = just("{").map(|_| Token::LBrace);
    let rbrace = just("}").map(|_| Token::RBrace);
    let lparen = just("(").map(|_| Token::LParen);
    let rparen = just(")").map(|_| Token::RParen);

    let token = num
        .or(plus)
        .or(minus)
        .or(and)
        .or(or)
        .or(le)
        .or(lt)
        .or(ge)
        .or(gt)
        .or(eq_eq)
        .or(not)
        .or(eq)
        .or(semicolon)
        .or(while_kw)
        .or(if_kw)
        .or(let_kw)
        .or(print_kw)
        .or(identifier)
        .or(lbrace)
        .or(rbrace)
        .or(lparen)
        .or(rparen);

    token.padded().repeated()
}

pub fn parse(input: String) -> Vec<Stmt> {
    let lexed = lexer().parse(input).unwrap();
    stmt_parser().repeated().parse(lexed).unwrap()
}

fn stmt_parser() -> impl Parser<Token, Stmt, Error=Simple<Token>> {
    recursive(|parser| {
        let ident = filter_map(|span, i| match i {
            Token::Identifier(identifier) => Ok(Identifier(identifier)),
            _ => Err(Simple::custom(span, "identifier expected")),
        });
        let print = just(Token::Print)
            .then(expr_parser())
            .map(|(_, expr)| Stmt::Print(expr));
        let var_decl = just(Token::Let)
            .then(ident)
            .map(|(_, ident)| Stmt::VarDecl(ident));
        let assign = ident
            .then(just(Token::Eq))
            .then(expr_parser())
            .map(|((ident, _), expr)| Stmt::Assign(ident, expr));
        let r#if = just(Token::If)
            .then(expr_parser().delimited_by(just(Token::LParen), just(Token::RParen)))
            .then(parser.clone().repeated().delimited_by(just(Token::LBrace), just(Token::RBrace)))
            .map(|((_, expr), block)| Stmt::If(expr, block));
        let r#while = just(Token::While)
            .then(expr_parser().delimited_by(just(Token::LParen), just(Token::RParen)))
            .then(parser.clone().repeated().delimited_by(just(Token::LBrace), just(Token::RBrace)))
            .map(|((_, expr), block)| Stmt::While(expr, block));
        print.or(var_decl).or(assign).or(r#if).or(r#while).then_ignore(just(Token::Semicolon))
    })
}

macro_rules! binop {
    ($parser:ident, $token_and_expr:ident) => {
        binop!($parser, $token_and_expr -> $token_and_expr)
    };
    ($parser:ident, $token:ident -> $expr:ident) => {
        binop_parser(
            $parser.clone(),
            Token::$token,
            |lhs, rhs| Expr::$expr(Box::new((lhs, rhs))),
        )
    };
}

// Note: it's not recursive :) please don't try `1 + 1 + 1`
fn expr_parser() -> impl Parser<Token, Expr, Error=Simple<Token>> {
    let literal = filter_map(|span, i| match i {
        Token::Literal(num) => Ok(Expr::Literal(num)),
        _ => Err(Simple::custom(span, "number expected")),
    });
    let ident = filter_map(|span, i| match i {
        Token::Identifier(identifier) => Ok(Identifier(identifier)),
        _ => Err(Simple::custom(span, "identifier expected")),
    });
    let var_ref = ident.map(|i| Expr::VarRef(i));
    let var_ref_or_literal = var_ref.or(literal);
    let sum = binop!(var_ref_or_literal, Plus -> Sum);
    let sub = binop!(var_ref_or_literal, Minus -> Sub);
    let and = binop!(var_ref_or_literal, And);
    let or = binop!(var_ref_or_literal, Or);
    let lt = binop!(var_ref_or_literal, LT);
    let le = binop!(var_ref_or_literal, LE);
    let gt = binop!(var_ref_or_literal, GT);
    let ge = binop!(var_ref_or_literal, GE);
    let eq_eq = binop!(var_ref_or_literal, EqEq -> Eq);
    let not = just(Token::Not).then(var_ref_or_literal).map(|(_, r)| Expr::Not(Box::new(r)));
    sum.or(sub).or(and).or(or)
        .or(lt).or(le).or(gt).or(ge).or(eq_eq)
        .or(not).or(var_ref_or_literal)
}

fn binop_parser(
    parser: impl Parser<Token, Expr, Error=Simple<Token>> + Clone,
    op: Token,
    create: impl Fn(Expr, Expr) -> Expr + 'static
) -> impl Parser<Token, Expr, Error=Simple<Token>> {
    parser.clone()
        .then(just(op))
        .then(parser)
        .map(move |((lhs, _), rhs)| create(lhs, rhs))
}
