# Note

This hw is still in a WIP state. So, I'll add more code and tests before hard deadline.
Currently there are no tests, except for gcd sample located [here](./test_scripts/simple_test).
$\gcd(44, 22) = 22$.

CPU isn't really optimized, but every instruction is only 2 bytes long!
`NOT` instruction is currently missing, however, it will be added as `SUB(-1, reg)`. And `-1` will be stored in a reserved register. So, when cpu notices that `r1 = -1` it will write result into `r2` (this is the only exceptional operation).

Also, if all variables are letters, then there are only 27 of them, so we can keep them as registers and don't really need memory for that. However, I do support load and store instructions. All integers are treated by my parser as strings by assembly. So, they are loaded into memory and loaded when needed.
Therefore, `a = 10` is `set a 10` which translates to `ld a ...` during compilation.

# Instructions

Load: 2 bytes for op code, 6 bytes for register id, 8 bytes for memory address.

- Load data from address to a specified register

Store: 2 bytes for op code, 6 bytes for register id, 8 bytes for memory address.

- Store data which is located in a specified register to given address.

Jump: 2 bytes for op code, 6 bytes for register id, 8 bytes instruction offset.

- If data in a given register is not 0, then jump by offset (can be negative).

Add/Sub/Or/And: 2 bytes for op code, 6 bytes for register id, 6 bytes for another register, 2 bytes for alu code.

- Perform some operation in ALU on 2 given registers and store the result in the first register.

Yes, there is no `c = a + b`, however, it can be replaced with `c = 0`, `c += a`, `c += b`.

# Commands for parser

- `ld reg addr` - load
- `str reg addr` - store
- `add reg1 reg2` - add, same for `sub`, `and`, `or`
- `set reg int` - special command to set the initial value of the variable
- `print reg reg` - fake print (yes, register should be typed twice, sorry for that)
- `jmp reg offset` - jump

# How to run

```bash
make
./build/cpu
```

If this is run from the `HW1` directory, it should print 22.
