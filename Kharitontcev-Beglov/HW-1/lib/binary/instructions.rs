#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Instr {
    R(RType),
    J(JType),
    I(IType),
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub struct RType {
    pub rs: u8,
    // 5
    pub rt: u8,
    // 5
    pub rd: u8,
    // 5
    pub funct: u8, // 6
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum JType {
    Jmp { address: u32 },
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum IType {
    Bne { rs: u8, rt: u8, imm: u16 },
    Beq { rs: u8, rt: u8, imm: u16 },
    Lw { rs: u8, rt: u8, imm: u16 },
    Sw { rs: u8, rt: u8, imm: u16 },
}

fn truncate_register(reg: u8) -> u8 {
    reg & 0x1f
}

fn transform_r_type(r_type: &RType) -> u32 {
    let rs = truncate_register(r_type.rs) as u32;
    let rt = truncate_register(r_type.rt) as u32;
    let rd = truncate_register(r_type.rd) as u32;
    (0u32 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (0u32 << 6) | (r_type.funct as u32)
}

fn transform_j_type(j_type: &JType) -> u32 {
    match j_type {
        JType::Jmp { address } => {
            let opcode = 2u32;
            (opcode << 26) | (address & 0x3ffffff)
        }
    }
}

fn transform_i_type(i_type: &IType) -> u32 {
    let (opcode, rs, rt, imm) = match i_type {
        IType::Bne { rs, rt, imm } => {
            (4u32, truncate_register(*rs) as u32, truncate_register(*rt) as u32, *imm as u32)
        }
        IType::Beq { rs, rt, imm } => {
            (5u32, truncate_register(*rs) as u32, truncate_register(*rt) as u32, *imm as u32)
        }
        IType::Sw { rs, rt, imm } => {
            (43u32, truncate_register(*rs) as u32, truncate_register(*rt) as u32, *imm as u32)
        }
        IType::Lw { rs, rt, imm } => {
            (34u32, truncate_register(*rs) as u32, truncate_register(*rt) as u32, *imm as u32)
        }
    };
    (opcode << 26) | (rs << 21) | (rt << 16) | imm
}

pub fn transform_to_bytes(instr: &Instr) -> u32 {
    match instr {
        Instr::R(r_type) => transform_r_type(r_type),
        Instr::J(j_type) => transform_j_type(j_type),
        Instr::I(i_type) => transform_i_type(i_type)
    }
}