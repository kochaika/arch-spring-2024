package org.example

abstract class Instruction {
    abstract val name : String
}

enum class Funct(val value : Int) {
    ADD(0b100000), AND(0b100100), DIV(0b011010), MULT(0b011000),
    NOR(0b100111), OR(0b100101), SLT(0b101010), SLL(0b000000),
    SRA(0b000011), SRL(0b000010), SUB(0b100010), XOR(0b100110),
}

val opcodeToInt = mapOf(
    "R_TYPE" to 0b000000,
    
    "ADDI" to 0b001000,
    "ANDI" to 0b001100,
    "ORI" to 0b001101,
    "XORI" to 0b001110,
    
    "LHI" to 0b011001,
    "LLO" to 0b011000,
    
    "SLTI" to 0b001010,
    
    "BEQ" to 0b000100,
    "BGTZ" to 0b000111,
    "BLEZ" to 0b000110,
    "BNE" to 0b000101,
    
    "J" to 0b000010,
    "JR" to 0b001000,
    
    "LW" to 0b100011,
    "SW" to 0b101011,
)

data class RType(
    override val name : String, val rs : Int, val rt : Int, val rd : Int, val shamt : Int = 0, val funct : Funct
) : Instruction()


data class IType(override val name : String, val rs : Int, val rt : Int, val imm : UShort) : Instruction()
data class JType(override val name : String = "j", val addr : Int) : Instruction()
data class Print(override val name : String = "print") : Instruction()


fun Instruction.toBinaryString() : String {
    return when (this) {
        is RType -> {
            val opcode = "000000"
            
            val rsBinary = rs.toString(2).padStart(5, '0')
            val rtBinary = rt.toString(2).padStart(5, '0')
            val rdBinary = rd.toString(2).padStart(5, '0')
            val shamtBinary = shamt.toString(2).padStart(5, '0')
            val functBinary = funct.value.toString(2).padStart(6, '0')
            
            opcode + rsBinary + rtBinary + rdBinary + shamtBinary + functBinary
        }
        
        is IType -> {
            val opcodeBinary = opcodeToInt[name.uppercase()]?.toString(2) ?: error("unknown OPCODE")
            val rsBinary = rs.toString(2).padStart(5, '0')
            val rtBinary = rt.toString(2).padStart(5, '0')
            val immBinary = imm.toString(2).padStart(16, '0')
            
            opcodeBinary + rsBinary + rtBinary + immBinary
        }
        
        is JType -> {
            "000010" + addr.toUInt().toString(2).padStart(26, '0').takeLast(26)
        }
        
        is Print -> {
            "1".repeat(32)
        }
        
        else -> TODO()
    }
}

fun Instruction.toInt() = this.toBinaryString().toUInt(2)
