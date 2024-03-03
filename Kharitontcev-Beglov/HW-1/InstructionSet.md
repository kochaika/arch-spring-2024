# Instruction set

The instruction set implement RISC's set of command, namely it's a reduced MIPS dataset.

We use the same mapping for the instruction, but just reduced supported instruction set. **If I've understood the task incorrectly tell me about it, please**.

<table>
        <thead>
          <tr>
            <td>Type</td>
            <td class="w6"><span class="l">31</span> <span class="r">26</span></td>
            <td class="w5"><span class="l">25</span> <span class="r">21</span></td>
            <td class="w5"><span class="l">20</span> <span class="r">16</span></td>
            <td class="w5"><span class="l">15</span> <span class="r">11</span></td>
            <td class="w5"><span class="l">10</span> <span class="r">06</span></td>
            <td class="w6"><span class="l">05</span> <span class="r">00</span></td>
          </tr>      
        </thead>      
        <tbody>
          <tr>
            <td>R-Type</td>
            <td>opcode</td>
            <td><span class="reg">$rs</span></td>
            <td><span class="reg">$rt</span></td>
            <td><span class="reg">$rd</span></td>
            <td><span class="shm">shamt</span></td>
            <td>funct</td>
          </tr>
          <tr>
            <td>I-Type</td>
            <td>opcode</td>
            <td><span class="reg">$rs</span></td>
            <td><span class="reg">$rt</span></td>
            <td colspan="3"><span class="imm">imm</span></td>
          </tr>
          <tr>
            <td>J-Type</td>
            <td>opcode</td>
            <td colspan="5"><span class="adr">address</span></td>
          </tr>      
        </tbody>
      </table>

# R-type instructions
`opcode=0`
## add
`$rd = $rs + $rt`, `funct  = 32`
## sub
`$rd = $rs - $rt`, `funct = 34`
## and
`$rd = $rs & $rt`, `funct = 36`
## or
`$rd = $rs | $rt`, `funct = 37`
## nor
`$rd = !($rs | $rt)`, `funct = 39`
## slt
`$rd = $rs < $rt ? 1 : 0`, `funct = 42`

# J-Type instruction
```
PC = {(PC + 4)[31:28], address, 00}
```
`opcode = 02`
# I-Type instruction
## beq
```
if ($rs = $rt)
    PC = PC + 4 + SignExt({imm, 00}) 
```
`opcode = 4` 
## bne
```
if ($rs != $rt)
    PC = PC + 4 + SignExt({imm, 00}) 
```
`opcode = 5` 

