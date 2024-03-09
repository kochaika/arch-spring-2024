mod emulator;
extern crate klang_lib;

use std::fs::File;
use std::io;
use std::io::Read;
use std::path::PathBuf;
use clap::Parser;
use crate::emulator::{Emulator, MEMORY_SIZE};

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    /// Set the compiled code file to run
    #[arg(short, long, value_name = "INPUT_CODE_FILE")]
    code: PathBuf,

    /// Set the compiled memory file to run
    #[arg(short, long, value_name = "INPUT_MEMORY_FILE")]
    memory: PathBuf,
    
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
    let cli = Cli::parse();
    read_checks(&cli.code);
    read_checks(&cli.memory);
    let mut code_file = File::open(cli.code)?;
    let mut code_buffer = Vec::new();
    code_file.read_to_end(&mut code_buffer)?;
    
    let mut memory_file = File::open(cli.memory)?;
    let mut memory_buffer = Vec::new();
    memory_file.read_to_end(&mut memory_buffer)?;
    let mut memory = [0u8; MEMORY_SIZE];
    memory[0..memory_buffer.len()].copy_from_slice(&memory_buffer);
    
    let mut emulator = Emulator::new(code_buffer, memory);
    
    loop {
        if emulator.clock() {
            break;
        }
    }
    
    Ok(())
}