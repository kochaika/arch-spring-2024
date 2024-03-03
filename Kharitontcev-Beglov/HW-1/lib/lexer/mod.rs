pub mod tokens;


use std::string::FromUtf8Error;
use tokens::*;
use nom::*;
use nom::branch::alt;
use nom::combinator::{map, map_res, recognize};
use nom::bytes::complete::{tag, take};
use nom::character::complete::{alpha1, i32};
use nom::error::ParseError;
use nom::multi::many0;
use nom::sequence::{delimited, pair};
macro_rules! syntax {
    ($func_name: ident, $tag_string: literal, $output_token: expr) => {
        fn $func_name<'a>(s: &'a [u8]) -> IResult<&[u8], Token> {
            map(tag($tag_string), |_| $output_token)(s)
        }
    };
}

syntax! {plus_operator, "+", Token::Plus}
syntax! {minus_operator, "-", Token::Minus}
syntax! {bitwise_and_operator, "&", Token::Ampersand}
syntax! {bitwise_or_operator, "|", Token::Pipe}
syntax! {bitwise_not_operator, "~", Token::Tilde}
syntax! {assign_operator, "=", Token::Assign}

pub fn lex_operator(input: &[u8]) -> IResult<&[u8], Token> {
    alt((
        assign_operator,
        plus_operator,
        minus_operator,
        bitwise_and_operator,
        bitwise_or_operator,
        bitwise_not_operator,
    ))(input)
}

syntax! {comma_punct, ",", Token::Comma}
syntax! {semicolon_punct, ";", Token::Semicolon}
syntax! {eol_punct, "\n", Token::EOL}
syntax! {lbrace_punct, "(", Token::LeftBrace}
syntax! {rbrace_punct, ")", Token::RightBrace}
syntax! {lparen_punct, "{", Token::OpenParenthesis}
syntax! {rparen_punct, "}", Token::CloseParenthesis}

pub fn lex_punct(input: &[u8]) -> IResult<&[u8], Token> {
    alt((
        comma_punct,
        semicolon_punct,
        eol_punct,
        lbrace_punct,
        rbrace_punct,
        lparen_punct,
        rparen_punct,
    ))(input)
}

fn complete_byte_slice_str_from_utf8(c: &[u8]) -> Result<String, FromUtf8Error> {
    String::from_utf8(c.to_vec())
}

pub fn lex_reserved_ident(input: &[u8]) -> IResult<&[u8], Token> {
    map_res(
        recognize(pair(
            alt((alpha1, tag("_"))),
            many0(alt((alpha1, tag("_")))),
        )),
        |s| {
            let c = complete_byte_slice_str_from_utf8(s);
            c.map(|syntax| match syntax.as_str() {
                "if" => Token::IfKeyword,
                "else" => Token::ElseKeyword,
                "while" => Token::WhileKeyword,
                "print" => Token::PrintKeyword,
                "var" => Token::VarKeyword,
                _ => Token::Ident(syntax.to_string()),
            })
        },
    )(input)
}

pub fn lex_int_literal(input: &[u8]) -> IResult<&[u8], Token> {
    map(i32, Token::IntLiteral)(input)
}

fn lex_illegal(input: &[u8]) -> IResult<&[u8], Token> {
    map(take(1usize), |_| Token::Illegal)(input)
}

fn lex_token(input: &[u8]) -> IResult<&[u8], Token> {
    alt((
        lex_operator,
        lex_punct,
        lex_reserved_ident,
        lex_int_literal,
        lex_illegal,
    ))(input)
}
fn multispace0<T, E: ParseError<T>>(input: T) -> IResult<T, T, E>
    where
        T: InputTakeAtPosition,
        <T as InputTakeAtPosition>::Item: AsChar + Clone,
{
    input.split_at_position_complete(|item| {
        let c = item.as_char();
        !(c == ' ' || c == '\t' || c == '\r')
    })
}
fn lex_tokens(input: &[u8]) -> IResult<&[u8], Vec<Token>> {
    many0(delimited(multispace0, lex_token, multispace0))(input)
}

pub struct Lexer;

impl Lexer {
    pub fn lex_tokens(bytes: &[u8]) -> IResult<&[u8], Vec<Token>> {
        lex_tokens(bytes)
            .map(|(slice, result)| (slice, [&result[..], &vec![Token::EOF][..]].concat()))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_lexer1() {
        let input = &b"=+(){},;"[..];
        let (_, result) = Lexer::lex_tokens(input).unwrap();

        let expected_results = vec![
            Token::Assign,
            Token::Plus,
            Token::LeftBrace,
            Token::RightBrace,
            Token::OpenParenthesis,
            Token::CloseParenthesis,
            Token::Comma,
            Token::Semicolon,
            Token::EOF,
        ];

        assert_eq!(result, expected_results);
    }
    
    #[test]
    fn test_eol() {
        let input = "\n".as_bytes();
        let (_, result) = Lexer::lex_tokens(input).unwrap();
        let expected_result = vec![Token::EOL, Token::EOF];
        assert_eq!(result, expected_result)
    }
    #[test]
    fn test_lexer2() {
        let input = "var a, b;
            a = 10
            b = 20
            if (a - b) {
                print a
            }
            while (a) { a = a - 1; print b; }
        ".as_bytes();
        let (_, result) = Lexer::lex_tokens(input).unwrap();
        let expected_results = vec![
            Token::VarKeyword, 
            Token::Ident(String::from("a")), 
            Token::Comma, 
            Token::Ident(String::from("b")), 
            Token::Semicolon, 
            Token::EOL,
            Token::Ident(String::from("a")), 
            Token::Assign, 
            Token::IntLiteral(10),
            Token::EOL,
            Token::Ident(String::from("b")), 
            Token::Assign, 
            Token::IntLiteral(20),
            Token::EOL,
            Token::IfKeyword, 
            Token::LeftBrace, 
            Token::Ident(String::from("a")), 
            Token::Minus, 
            Token::Ident(String::from("b")), 
            Token::RightBrace, 
            Token::OpenParenthesis,
            Token::EOL,
            Token::PrintKeyword, 
            Token::Ident(String::from("a")), 
            Token::EOL, 
            Token::CloseParenthesis,
            Token::EOL,
            Token::WhileKeyword, 
            Token::LeftBrace, 
            Token::Ident(String::from("a")), 
            Token::RightBrace, 
            Token::OpenParenthesis,
            Token::Ident(String::from("a")), 
            Token::Assign, 
            Token::Ident(String::from("a")), 
            Token::Minus, 
            Token::IntLiteral(1), 
            Token::Semicolon, 
            Token::PrintKeyword, 
            Token::Ident(String::from("b")), 
            Token::Semicolon, 
            Token::CloseParenthesis,
            Token::EOL,
            Token::EOF
        ];
        
        assert_eq!(result, expected_results);
    }
}