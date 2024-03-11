pub mod ast;

use nom::branch::alt;
use nom::bytes::complete::take;
use nom::{error_position, IResult};
use crate::lexer::tokens::{Token, Tokens};
use crate::parser::ast::*;
use nom::Err;
use nom::error::{Error, ErrorKind};
use nom::combinator::{map, opt, verify};
use nom::multi::{many0, many1, separated_list0, separated_list1};
use nom::sequence::{delimited, terminated, tuple};
use crate::parser::ast::Expr::InfixOperation;

macro_rules! tag_token (
    ($func_name: ident, $tag: expr) => (
        fn $func_name(tokens: Tokens) -> IResult<Tokens, Tokens> {
            verify(take(1usize), |t: &Tokens| t.tok[0] == $tag)(tokens)
        }
    )
  );

fn parse_literal(tokens: Tokens) -> IResult<Tokens, Expr> {
    let (i1, t1) = take(1usize)(tokens)?;
    if t1.tok.is_empty() {
        Err(Err::Error(Error::new(tokens, ErrorKind::Tag)))
    } else {
        match t1.tok[0].clone() {
            Token::IntLiteral(x) => Ok((i1, Expr::IntLiteral(x))),
            _ => Err(Err::Error(Error::new(tokens, ErrorKind::Tag)))
        }
    }
}

fn parse_ident(input: Tokens) -> IResult<Tokens, Ident> {
    let (remaining, token) = take(1usize)(input)?;
    if token.tok.is_empty() {
        Err(Err::Error(Error::new(input, ErrorKind::Tag)))
    } else {
        match token.tok[0].clone() {
            Token::Ident(name) => Ok((remaining, Ident(name))),
            _ => Err(Err::Error(Error::new(input, ErrorKind::Tag)))
        }
    }
}

tag_token!(assign_tag, Token::Assign);
tag_token!(lbrace_tag, Token::LeftBrace);
tag_token!(rbrace_tag, Token::RightBrace);
tag_token!(lparen_tag, Token::OpenParenthesis);
tag_token!(rparen_tag, Token::CloseParenthesis);
tag_token!(comma_tag, Token::Comma);
tag_token!(not_tag, Token::Tilde);
tag_token!(if_tag, Token::IfKeyword);
tag_token!(else_tag, Token::ElseKeyword);
tag_token!(while_tag, Token::WhileKeyword);
tag_token!(print_tag, Token::PrintKeyword);
tag_token!(var_tag, Token::VarKeyword);
tag_token!(eof_tag, Token::EOF);

fn infix_op(t: &Token) -> (Precedence, Option<Ops>) {
    match *t {
        Token::Plus => (Precedence::Operations, Some(Ops::Add)),
        Token::Minus => (Precedence::Operations, Some(Ops::Sub)),
        Token::Ampersand => (Precedence::BitOperations, Some(Ops::BitwiseAnd)),
        Token::Pipe => (Precedence::BitOperations, Some(Ops::BitwiseOr)),
        _ => (Precedence::Lowest, None)
    }
}

fn parse_atom_expr(input: Tokens) -> IResult<Tokens, Expr> {
    alt((
        parse_prefix_expr,
        parse_literal,
        parse_ident_expr,
        delimited(lbrace_tag, parse_full_expr, rbrace_tag)
    ))(input)
}


fn parse_ident_expr(tokens: Tokens) -> IResult<Tokens, Expr> {
    map(parse_ident, Expr::Var)(tokens)
}

fn parse_prefix_expr(input: Tokens) -> IResult<Tokens, Expr> {
    let (remaining, token) = not_tag(input)?;
    if token.tok.is_empty() {
        Err(Err::Error(error_position!(input, ErrorKind::Tag)))
    } else {
        let (i2, e) = parse_atom_expr(remaining)?;
        match token.tok[0].clone() {
            Token::Tilde => Ok((i2, Expr::PrefixOperation(PrefixOps::BitwiseNot, Box::new(e)))),
            Token::Minus => Ok((i2, Expr::PrefixOperation(PrefixOps::UnaryMinus, Box::new(e)))),
            _ => Err(Err::Error(error_position!(input, ErrorKind::Tag))),
        }
    }
}

fn parse_full_expr(input: Tokens) -> IResult<Tokens, Expr> {
    parse_expr(input, Precedence::Lowest)
}

fn parse_expr(input: Tokens, precedence: Precedence) -> IResult<Tokens, Expr> {
    let (remaining, expr) = parse_atom_expr(input)?;
    go_parse_expr(remaining, precedence, expr)
}

fn go_parse_expr(input: Tokens, precedence: Precedence, left: Expr) -> IResult<Tokens, Expr> {
    let (i1, t1) = take(1usize)(input)?;

    if t1.tok.is_empty() {
        Ok((i1, left))
    } else {
        let preview = &t1.tok[0];
        let (ref prec, _) = infix_op(preview);
        if precedence < *prec {
            let (remaining2, left2) = parse_infix(input, left)?;
            go_parse_expr(remaining2, precedence, left2)
        } else {
            Ok((input, left))
        }
    }
}

fn parse_infix(input: Tokens, left: Expr) -> IResult<Tokens, Expr> {
    let (i1, t1) = take(1usize)(input)?;
    if t1.tok.is_empty() {
        Err(Err::Error(error_position!(input, ErrorKind::Tag)))
    } else {
        let next = &t1.tok[0];
        let (precedence, maybe_op) = infix_op(next);
        match maybe_op {
            None => Err(Err::Error(error_position!(input, ErrorKind::Tag))),
            Some(op) => {
                let (i2, right) = parse_expr(i1, precedence)?;
                Ok((i2, InfixOperation(Box::new(left), op, Box::new(right))))
            }
        }
    }
}

fn parse_var_declaration(input: Tokens) -> IResult<Tokens, Stmt> {
    map(tuple((
        var_tag,
        separated_list1(comma_tag, parse_ident)
    )),
        |(_, list)| Stmt::VarDeclaration(list),
    )(input)
}

fn parse_var_assign(input: Tokens) -> IResult<Tokens, Stmt> {
    map(tuple((
        parse_ident,
        assign_tag,
        parse_full_expr,
    )),
        |(ident, _, expr)| Stmt::VarAssign(ident, expr))(input)
}

fn parse_if(input: Tokens) -> IResult<Tokens, Stmt> {
    map(
        tuple((
            if_tag,
            lbrace_tag,
            parse_full_expr,
            rbrace_tag,
            parse_block,
            opt(tuple((else_tag, parse_block)))
        )),
        |(_, _, condition, _, true_block, else_block)| {
            Stmt::If {
                condition,
                true_branch: Box::new(true_block),
                false_branch: match else_block {
                    None => None,
                    Some((_, else_block)) => Some(Box::new(else_block))
                },
            }
        })(input)
}

fn parse_while(input: Tokens) -> IResult<Tokens, Stmt> {
    map(
        tuple((
            while_tag,
            lbrace_tag,
            parse_full_expr,
            rbrace_tag,
            parse_block
        )),
        |(_, _, condition, _, body)| Stmt::While(condition, Box::new(body)),
    )(input)
}

fn parse_print(input: Tokens) -> IResult<Tokens, Stmt> {
    map(
        tuple((
            print_tag,
            lbrace_tag,
            parse_full_expr,
            rbrace_tag
        )),
        |(_, _, expr, _)| Stmt::Print(expr)
    )(input)
}

fn parse_stmt(input: Tokens) -> IResult<Tokens, Stmt> {
    alt((
        parse_var_declaration,
        parse_var_assign,
        parse_if,
        parse_while,
        parse_print
    ))(input)
}

fn stmt_separator(input: Tokens) -> IResult<Tokens, Tokens> {
    verify(take(1usize), |t: &Tokens| t.tok[0] == Token::EOL || t.tok[0] == Token::Semicolon)(input)
}

fn parse_block(input: Tokens) -> IResult<Tokens, Block> {
    delimited(lparen_tag,
              delimited(many0(stmt_separator),
                        separated_list0(many1(stmt_separator), parse_stmt),
                        many0(stmt_separator)),
              rparen_tag,
    )(input)
}

pub struct Parser;
impl Parser {
    pub fn parse(tokens: Tokens) -> IResult<Tokens, Block> {
        terminated(parse_block, eof_tag)(tokens)
    }
}

#[cfg(test)]
mod tests {
    use crate::lexer::{Lexer, tokens::Tokens};
    use crate::parser::ast::PrefixOps::BitwiseNot;
    use super::*;

    #[test]
    fn test_expr() {
        let input = "~1&(3+4)".as_bytes();
        let (_, lexed) = Lexer::lex_tokens(input).unwrap();
        let (_, parsed) = parse_full_expr(Tokens::new(lexed.as_slice())).unwrap();
        assert_eq!(parsed, Expr::InfixOperation(
            Box::new(Expr::PrefixOperation(BitwiseNot, Box::new(Expr::IntLiteral(1)))),
            Ops::BitwiseAnd,
            Box::new(Expr::InfixOperation(
                Box::new(Expr::IntLiteral(3)),
                Ops::Add,
                Box::new(Expr::IntLiteral(4)),
            )),
        ))
    }
}