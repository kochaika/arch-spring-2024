use std::fs::File;
use std::io;
use std::io::{Read, Write};
use std::path::PathBuf;
use clap::Parser;
use klang_lib::binary::SMTransformer;
use klang_lib::lexer::Lexer;
use klang_lib::lexer::tokens::Tokens;
use klang_lib::stack_machine::transform::AstTransformer;

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    /// Set the source code file to compile
    #[arg(short, long, value_name = "SOURCE_CODE_FILE")]
    input: PathBuf,
    
    /// Output file for the compiled code
    #[arg(short, long, value_name = "CODE_BINARY")]
    code: PathBuf,
    
    /// Output file for the compiled memory
    #[arg(short, long, value_name = "MEMORY_BINARY")]
    memory: PathBuf
}

fn read_checks(file: &PathBuf) {
    if !file.exists() {
        panic!("File {} is not exists", file.as_path().to_str().unwrap())
    }
    if !file.is_file() {
        panic!("File {} is not a file", file.as_path().to_str().unwrap())
    }
}

fn main() -> io::Result<()> {
    let mut ast_transformer = AstTransformer::new();
    let mut stack_machine_transformer = SMTransformer::new();
    
    let cli = Cli::parse();
    read_checks(&cli.input);
    let mut input_file = File::open(cli.input)?;
    let mut source_code_buffer = Vec::new();
    input_file.read_to_end(&mut source_code_buffer)?;
    let (left, lexed) = Lexer::lex_tokens(source_code_buffer.as_slice()).unwrap();
    if !left.is_empty() {
        panic!("Not all source code parsed!");
    }
    let (_, parsed) = klang_lib::parser::Parser::parse(Tokens::new(&lexed)).unwrap();
    let stack_machine = ast_transformer.transform_ast_to_sm(parsed);
    let (memory, code) = stack_machine_transformer.transform_program(&stack_machine);
    
    let mut code_file = File::create(cli.code)?;
    code_file.write_all(&code)?;
    let mut memory_file = File::create(cli.memory)?;
    memory_file.write_all(&memory)
}
