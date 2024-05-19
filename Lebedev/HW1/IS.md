# Instructions
All instructions have total size of 6 bytes. 
Sometimes some remaining bytes are unused.

Note: integers are always stored in native platform endianness

## ALU Instructions
All ALU instructions have the following format:
- first byte: kind code of alu instruction
- second byte: register number where will result be put
- third byte: first operand of ALU
- fourth byte: second operand of ALU (if applicable)

Full list of ALU instructions (`kind code => kind`) (snippet is from `/interpreter/mod.rs`):
```
    0 => ADD
    1 => SUB
    2 => MUL
    3 => DIV
    4 => REM

    5 => SHL
    6 => SHR

    7 => EQ
    8 => LT

    9 => OR
    10 => AND
    11 => NOT

    17 => PRINT
```

## Jump Instructions
- first byte: kind code of jump instruction
- second byte: register to be checked against (used only for conditional jump)
- all other bytes is address of program to where jump should be performed. 

Full list of Jump instructions (`kind code => kind`) (snippet is from `/interpreter/mod.rs`):
```
    12 => JUMP
    13 => CJUMP // aka jump if zero
```

## Memory instructions
- first byte: kind code of memory instruction
- second byte: register number. Used only for kind `Store`. The needed value is stored here 
- all other bytes is address. More information below

### Kind `Store` = 14
Writes data in memory (of variable by index)

### Kind `LoadStack` = 15
Reads data from memory (of variable by index)

### Kind `LoadConst` = 16
Reads constant of program

# Notes on lang:
It's bad. Awful even. Won't even say that it could not parse input.
On valid code it should work. But there is no specification of valid code.
Also, to write negative numbers you have a special format: `0 - 3` means `-3`.

Usually, you might forget semicolon at the end of every statement 
(including while)
