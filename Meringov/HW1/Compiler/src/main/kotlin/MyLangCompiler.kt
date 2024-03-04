package org.example

import MyLangBaseVisitor
import MyLangParser

abstract class Instruction {
    abstract val name : String
}

data class Label(override val name : String) : Instruction()

enum class Funct(val value : Int) {
    ADD(0b100000),
    AND(0b100100),
    DIV(0b011010),
    MULT(0b011000),
    NOR(0b100111),
    OR(0b100101),
    SLT(0b101010),
    SLL(0b000000),  // $rd = $rt << SHAMT
    SRA(0b000011),  // $rd = $rt >> SHAMT
    SRL(0b000010),  // $rd = $rt >>> SHAMT
    SUB(0b100010),
    XOR(0b100110),
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
    override val name : String,
    val rs : Int,
    val rt : Int,
    val rd : Int,
    val shamt : Int = 0,
    val funct : Funct
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
            "000010" + addr.toString(2).padStart(26, '0')
        }
        
        is Print -> {
            "1".repeat(32)
        }
        
        else -> TODO()
    }
}

fun Instruction.toInt() = this.toBinaryString().toUInt(2)


const val stackPointer = 29
const val framePointer = 30
const val rresult = 2
const val zeroRegister = 0

val variables = mutableMapOf<String, Int>()

class MyLangCompiler : MyLangBaseVisitor<List<Instruction>>() {
    private fun push(rt : Int) : List<Instruction> {
        return listOf(
            IType("addi", rs = stackPointer, rt = stackPointer, imm = (-4).toUShort()),
            IType("sw", rs = stackPointer, rt = rt, 0u)
        )
    }
    
    
    private fun pop(rt : Int) : List<Instruction> {
        return listOf(
            IType("lw", rs = stackPointer, rt = rt, 0u),
            IType("addi", rs = stackPointer, rt = stackPointer, imm = 4u)
        )
    }
    
    override fun visitBlock(ctx : MyLangParser.BlockContext) : List<Instruction> {
        return ctx.children.map { visit(it) }.flatten()
    }
    
    override fun visitIntExpression(ctx : MyLangParser.IntExpressionContext) : List<Instruction> {
        val imm = ctx.INT().text.toInt()
        
        val hi = imm.ushr(16).toUShort()
        val lo = imm.toUShort()
        
        
        return listOf(
            IType("lhi", rs = zeroRegister, rt = rresult, imm = hi),
            IType("llo", rs = zeroRegister, rt = rresult, imm = lo)
        )
    }
    
    override fun visitAdditiveExpression(ctx : MyLangParser.AdditiveExpressionContext) : List<Instruction> {
        return visit(ctx.expression(0)) +
                push(rresult) +
                visit(ctx.expression(1)) +
                pop(8) +
                when {
                    ctx.PLUS() != null -> RType("add", rs = rresult, rt = 8, rd = rresult, shamt = 0, funct = Funct.ADD)
                    ctx.MINUS() != null -> RType(
                        "sub",
                        rs = 8,
                        rt = rresult,
                        rd = rresult,
                        shamt = 0,
                        funct = Funct.SUB
                    )
                    
                    else -> error("Unknown operation")
                }
    }
    
    override fun visitRelationalExpression(ctx : MyLangParser.RelationalExpressionContext) : List<Instruction> {
        return TODO("maybe later")
    }
    
    override fun visitWhile(ctx : MyLangParser.WhileContext?) : List<Instruction> {
        return TODO("maybe later")
    }
    
    override fun visitPrint(ctx : MyLangParser.PrintContext) : List<Instruction> {
        return visit(ctx.expression()) + Print()
    }
    
    override fun visitDeclaration(ctx : MyLangParser.DeclarationContext) : List<Instruction> {
        val name = ctx.VARIABLE().text
        if (variables.contains(name)) error("redeclaration of '$name'")
        
        variables[name] = variables.size * 4
        
        return push(zeroRegister)
    }
    
    override fun visitLogicalExpression(ctx : MyLangParser.LogicalExpressionContext) : List<Instruction> {
        println(ctx.children.map { it.text })
        return visit(ctx.expression(0)) +
                push(rresult) +
                visit(ctx.expression(1)) +
                pop(8) +
                when {
                    ctx.AND() != null -> RType("and", rs = rresult, rt = 8, rd = rresult, funct = Funct.AND)
                    ctx.OR() != null -> RType("or", rs = rresult, rt = 8, rd = rresult, funct = Funct.OR)
                    else -> error("Unknown operation")
                }
    }
    
    
    override fun visitProgram(ctx : MyLangParser.ProgramContext) : List<Instruction> {
        return visit(ctx.block())
    }
    
    override fun visitIf(ctx : MyLangParser.IfContext) : List<Instruction> {
        
        val falseBranch = (ctx.block(1)?.let { visit(it) } ?: listOf())
        val trueBranch = visit(ctx.block(0)) + JType(addr = falseBranch.size)
        

//        println("trueBranch = $trueBranch")
//        println("falseBranch = $falseBranch")
        
        
        val result =
            visit(ctx.expression()) +
                    IType(
                        "beq",
                        rs = rresult,
                        rt = zeroRegister,
                        imm = (trueBranch.size + 1).toUShort()
                    ) + trueBranch + falseBranch

//        println("result = $result")
        
        return result
        
    }
    
    override fun visitNotExpression(ctx : MyLangParser.NotExpressionContext) : List<Instruction> {
        return visit(ctx.getChild(1)) + RType("nor", rs = rresult, rt = zeroRegister, rd = rresult, funct = Funct.NOR)
    }
    
    override fun visitAssignment(ctx : MyLangParser.AssignmentContext) : List<Instruction> {
        val name = ctx.getChild(0).text
        
        return listOf(
            IType(
                "sw",
                rs = framePointer,
                rt = zeroRegister,
                imm = variables[name]?.toUShort() ?: error("variable '$name' is used before declaration")
            )
        )
    }
}