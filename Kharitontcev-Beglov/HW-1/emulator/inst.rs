use nom::combinator::map;
use nom::{bits::{bits, complete::take, complete::tag}, error_position, IResult};
use nom::branch::alt;
use nom::error::{ErrorKind};
use nom::multi::many1;
use nom::sequence::tuple;
use klang_lib::binary::instructions::{Instr, RType, JType, IType};
type BitStream<'a> = (&'a [u8], usize);

fn parse_r_type(input: BitStream) -> IResult<BitStream, RType> {
    map(tuple((
        tag(0x0, 6usize),
        take(5usize),
        take(5usize),
        take(5usize),
        take::<_, u8, _, _>(5usize),
        take(6usize)
    )), |(_, rs, rt, rd, _, funct)| RType { rs, rt, rd, funct })(input)
}

fn parse_jmp(input: BitStream) -> IResult<BitStream, JType> {
    map(tuple((
        tag(2, 6usize),
        take::<_, u8, _, _>(2usize),
        take::<_, u8, _, _>(8usize),
        take::<_, u8, _, _>(8usize),
        take::<_, u8, _, _>(8usize),
    )), |(_, p1, p2, p3, p4): (u8, u8, u8, u8, u8)| {
        let address = p4 as u32 + (p3 as u32 + (p2 as u32 + ((p1 as u32) << 8) << 8) << 8);
        JType::Jmp { address }
    })(input)
}

fn parse_i_type(input: BitStream) -> IResult<BitStream, IType> {
    let (r, (opcode, rs, rt, imm_1, imm_2))
        : (BitStream, (u8, u8, u8, u8, u8)) = tuple((
        take(6usize),
        take(5usize),
        take(5usize),
        take(8usize),
        take(8usize),
    ))(input)?;
    let imm = (imm_1 as u16) << 8 + imm_2 as u16;
    match opcode {
        0x4 => Ok((r, IType::Beq { rs, rt, imm })),
        0x5 => Ok((r, IType::Bne { rs, rt, imm })),
        0x22 => Ok((r, IType::Lw { rs, rt, imm})),
        0x2b => Ok((r, IType::Sw { rs, rt, imm})),
        _ => Err(nom::Err::Error(error_position!(input, ErrorKind::Tag)))
    }
}

fn parse_instr(input: BitStream) -> IResult<BitStream, Instr> {
    alt((
        map(parse_r_type, Instr::R),
        map(parse_i_type, Instr::I),
        map(parse_jmp, Instr::J)
    ))(input)
}

fn parse_instrs(input: BitStream) -> IResult<BitStream, Vec<Instr>> {
    many1(parse_instr)(input)
}

pub struct ProgramParser;

impl ProgramParser {
    pub fn parse(bytes: &[u8]) -> IResult<&[u8], Vec<Instr>> {
        bits(parse_instrs)(bytes)
    }
}
