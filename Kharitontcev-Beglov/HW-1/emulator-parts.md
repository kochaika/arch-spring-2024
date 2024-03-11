# Parts of the emulator

The emulator is split into the separate parts as they appear in a real CPU:

## ALU

The Arithmetic-logical implements different operation for the integer inputs. For the input two 32-bits input words are
requires. The output is a single word with a result. For control over ALU the function 5-bit line is provided. 

The actual functional values could be found in "R-type instructions" of the instruction set.

## Register file

The current implementation of the register file supports 32 registers. Some specific register are reserved for special purposes:
- 0th register: zero value. Can't be overridden. Always return a zero value
- 29th register: stack pointer. Points at the end of the stack. Stack manipulations are not implemented in the processors, thus arithmetics over this register is required.

Other registers are general purpose.

All registers are a single word.
