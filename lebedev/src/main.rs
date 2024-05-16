#![feature(box_patterns)]

use std::{fs, io};

use clap::{arg, ArgMatches, Command};
use crate::interpreter::{ActualProgram, Printer, Program, StdPrinter};

use crate::interpreter::machine::Machine;
use crate::lang::compiler;
use crate::lang::parser::parse;

mod interpreter;
mod lang;

fn main() -> io::Result<()> {
    let matches = Command::new("HW1")
        .subcommand(
            Command::new("interpret")
                .arg(arg!(-p --program <FILE> "path to file with the program"))
        )
        .subcommand(
            Command::new("compile")
                .arg(arg!(-s --source <FILE> "path to source file with the program"))
                .arg(arg!(-c --compiled <FILE> "path to place resulting compiled program"))
        )
        .get_matches();
    interpret(&matches)?;
    compile(&matches);
    Ok(())
}

fn interpret(matches: &ArgMatches) -> io::Result<()> {
    let Some(matches) = matches.subcommand_matches("interpret") else { return Ok(()) };
    let program_path = matches.get_one::<String>("program").unwrap();
    let program = ActualProgram::new(program_path)?;
    let mut machine: Machine<ActualProgram, StdPrinter> = Machine::new(program);
    start_machine(&mut machine);
    Ok(())
}

fn start_machine<P1: Program, P2: Printer + Default>(machine: &mut Machine<P1, P2>) {
    loop {
        if !machine.clock() {
            break
        }
    }
}

fn compile(matches: &ArgMatches) {
    let Some(matches) = matches.subcommand_matches("compile") else { return };
    let source_path = matches.get_one::<String>("source").unwrap();
    let compiled_path = matches.get_one::<String>("compiled").unwrap();
    let contents = fs::read_to_string(source_path).unwrap();
    let parsed = parse(contents);
    let compiled = compiler::compile(parsed);
    let buffer = compiled.crete_buffer();
    fs::write(compiled_path, buffer).unwrap();
}

#[cfg(test)]
mod tests {
    use crate::interpreter::{ActualProgram, Printer};
    use crate::interpreter::machine::Machine;
    use crate::lang::compiler::compile;
    use crate::lang::parser::parse;
    use crate::start_machine;

    #[test]
    fn test_simple_print() {
        test("print 42;", vec!["42"]);
    }

    #[test]
    fn test_if_positive() {
        test("\
            print 1;
            a = 5;
            b = 10;
            c = a + b;
            if (c > 12) {
                print c;
                print b;
            };
            a = a - a;
            print a;
            print 0;
        ", vec!["1", "15", "10", "0", "0"])
    }

    #[test]
    fn test_if_negative() {
        test("\
            print 1;
            a = 5;
            b = 10;
            c = a + b;
            if (c <= 12) {
                print c;
                print b;
            };
            a = a - a;
            print a;
            print 0;
        ", vec!["1", "0", "0"])
    }

    #[test]
    fn test_while() {
        test("\
            a = 10;
            b = 0 - 3;
            while (a >= b) {
                print a;
                a = a - 1;
            };
            print a;
        ", vec!["10", "9", "8", "7", "6", "5", "4", "3", "2", "1", "0", "-1", "-2", "-3", "-4"])
    }

    #[test]
    fn test_while_and_if() {
        test("\
            a = 7;
            while (0 < a) {
                b = a & 1;
                if (b == 0) {
                    print a;
                };
                a = a - 1;
            };
        ", vec!["6", "4", "2"])
    }

    #[derive(Default)]
    struct BufferPrinter {
        strings: Vec<String>,
    }

    impl Printer for BufferPrinter {
        fn print(&mut self, string: String) {
            self.strings.push(string);
        }
    }

    fn test(code: &str, expected: Vec<&str>) {
        let expected: Vec<_> = expected.iter().map(|s| s.to_string()).collect();
        let parsed = parse(code.to_string());
        let compiled = compile(parsed);
        let buffer = compiled.crete_buffer();
        let program = ActualProgram::from_bytes(buffer);
        let mut machine = Machine::<ActualProgram, BufferPrinter>::new(program);
        start_machine(&mut machine);
        let collected = &machine.alu_printer().strings;
        assert_eq!(expected, *collected);
    }
}
