For the simplification a compiled code is split into two parts: **Memory** and **Code**

# Memory

Saves as `<file>.mem`

The memory part contains the initial memory snapshot. 
Basically it contains all constants, that was in the code.
The reason for that is lack of I-type operations, so we need to load constant from memory to use them.

# Code

Saves as `<file>.bin`

The code part is an actually compiled into the instruction set instructions.

Since the RISC format is used all instructions has the same length.