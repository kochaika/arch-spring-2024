use crate::interpreter::{ALUInstructionKind, Word};
use crate::interpreter::alu::printers::Printer;
use crate::interpreter::registers::Registers;

#[derive(Default)]
pub struct ALU<P: Printer + Default> {
    printer: P,
}

pub mod printers {
    #[derive(Default)]
    pub struct StdPrinter;

    impl Printer for StdPrinter {
        fn print(&mut self, string: String) {
            println!("{}", string);
        }
    }

    pub trait Printer {
        fn print(&mut self, string: String);
    }
}

impl <P: Printer + Default> ALU<P> {
    pub fn evaluate(&mut self, registers: &mut Registers, kind: ALUInstructionKind) {
        let lhs = registers[Registers::alu_lhs()];
        let rhs = registers[Registers::alu_rhs()];
        let res = self.do_evaluate(kind, lhs, rhs);
        registers[Registers::alu_res()] = res;
    }

    pub fn printer(&self) -> &P {
        &self.printer
    }

    fn do_evaluate(
        &mut self,
        kind: ALUInstructionKind,
        lhs: Word,
        rhs: Word,
    ) -> Word {
        let lhs = lhs.0;
        let rhs = rhs.0;
        let res = match kind {
            ALUInstructionKind::ADD => lhs.wrapping_add(rhs),
            ALUInstructionKind::SUB => lhs.wrapping_sub(rhs),
            ALUInstructionKind::MUL => lhs * rhs,
            ALUInstructionKind::DIV => lhs / rhs,
            ALUInstructionKind::REM => lhs % rhs,
            ALUInstructionKind::SHL => lhs << rhs,
            ALUInstructionKind::SHR => lhs >> rhs,
            ALUInstructionKind::EQ => if lhs == rhs { 1 } else { 0 },
            ALUInstructionKind::LT => if lhs < rhs { 1 } else { 0 }
            ALUInstructionKind::OR => ((lhs as u32) | (rhs as u32)) as i32,
            ALUInstructionKind::AND => ((lhs as u32) & (rhs as u32)) as i32,
            ALUInstructionKind::NOT => !(lhs as u32) as i32,

            ALUInstructionKind::PRINT => {
                self.printer.print(format!("{}", lhs));
                lhs
            }
        };
        Word::new(res)
    }
}

#[cfg(test)]
mod tests {
    use crate::interpreter::{ALUInstructionKind, Word};
    use crate::interpreter::alu::{ALU, Printer};
    use crate::interpreter::registers::Registers;

    #[derive(Default)]
    struct EmptyPrinter;

    impl Printer for EmptyPrinter {
        fn print(&mut self, string: String) {}
    }

    const ALU: ALU<EmptyPrinter> = ALU { printer: EmptyPrinter };

    #[test]
    fn add_simple() {
        let mut registers = registers(1, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::ADD);
        assert_eq!(registers[Registers::alu_res()], Word(3));
    }

    #[test]
    fn add_overflow() {
        let mut registers = registers(i32::MAX, 1);
        ALU.evaluate(&mut registers, ALUInstructionKind::ADD);
        assert_eq!(registers[Registers::alu_res()], Word(i32::MIN));
    }

    #[test]
    fn sub_simple() {
        let mut registers = registers(5, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::SUB);
        assert_eq!(registers[Registers::alu_res()], Word::new(3));
    }
    
    #[test]
    fn sub_overflow() {
        let mut registers = registers(i32::MIN, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::SUB);
        assert_eq!(registers[Registers::alu_res()], Word::new(i32::MAX - 1));
    }

    #[test]
    fn mul_simple() {
        let mut registers = registers(5, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::MUL);
        assert_eq!(registers[Registers::alu_res()], Word::new(10));
    }

    #[test]
    fn div_simple() {
        let mut registers = registers(10, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::DIV);
        assert_eq!(registers[Registers::alu_res()], Word::new(5));
    }

    #[test]
    fn rem_simple() {
        let mut registers = registers(10, 3);
        ALU.evaluate(&mut registers, ALUInstructionKind::REM);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    #[test]
    fn shift_left_simple() {
        let mut registers = registers(1, 1);
        ALU.evaluate(&mut registers, ALUInstructionKind::SHL);
        assert_eq!(registers[Registers::alu_res()], Word::new(2));
    }

    #[test]
    fn shift_right_simple() {
        let mut registers = registers(2, 1);
        ALU.evaluate(&mut registers, ALUInstructionKind::SHR);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    #[test]
    fn eq_true() {
        let mut registers = registers(1, 1);
        ALU.evaluate(&mut registers, ALUInstructionKind::EQ);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    #[test]
    fn eq_false() {
        let mut registers = registers(1, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::EQ);
        assert_eq!(registers[Registers::alu_res()], Word::new(0));
    }

    #[test]
    fn lt_true() {
        let mut registers = registers(1, 2);
        ALU.evaluate(&mut registers, ALUInstructionKind::LT);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    #[test]
    fn lt_false() {
        let mut registers = registers(2, 1);
        ALU.evaluate(&mut registers, ALUInstructionKind::LT);
        assert_eq!(registers[Registers::alu_res()], Word::new(0));
    }

    #[test]
    fn or_simple() {
        let mut registers = registers(1, 0);
        ALU.evaluate(&mut registers, ALUInstructionKind::OR);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    #[test]
    fn and_simple() {
        let mut registers = registers(1, 1);
        ALU.evaluate(&mut registers, ALUInstructionKind::AND);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    #[test]
    fn not_simple() {
        let mut registers = registers(1, 0);
        ALU.evaluate(&mut registers, ALUInstructionKind::NOT);
        assert_eq!(registers[Registers::alu_res()], Word::new(-2));
    }

    #[test]
    fn print_simple() {
        let mut registers = registers(1, 0);
        ALU.evaluate(&mut registers, ALUInstructionKind::PRINT);
        assert_eq!(registers[Registers::alu_res()], Word::new(1));
    }

    fn registers(lhs: i32, rhs: i32) -> Registers {
        let mut registers = Registers::default();
        registers[Registers::alu_lhs()] = Word::new(lhs);
        registers[Registers::alu_rhs()] = Word::new(rhs);
        registers
    }
}
