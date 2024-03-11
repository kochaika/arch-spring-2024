package org.example

import MyLangBaseVisitor
import MyLangParser

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
            IType("lw", rs = stackPointer, rt = rt, 0u), IType("addi", rs = stackPointer, rt = stackPointer, imm = 4u)
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
        return visit(ctx.expression(0)) + push(rresult) + visit(ctx.expression(1)) + pop(8) + when {
            ctx.PLUS() != null -> RType("add", rs = rresult, rt = 8, rd = rresult, shamt = 0, funct = Funct.ADD)
            ctx.MINUS() != null -> RType(
                "sub", rs = 8, rt = rresult, rd = rresult, shamt = 0, funct = Funct.SUB
            )
            
            else -> error("Unknown operation")
        }
    }
    
    override fun visitRelationalExpression(ctx : MyLangParser.RelationalExpressionContext) : List<Instruction> {
        
        return visit(ctx.expression(0)) + push(rresult) + visit(ctx.expression(1)) + pop(8) + when {
            ctx.EQ() != null -> listOf(
                IType("beq", rs = 8, rt = rresult, imm = 2u),
                RType("or", rs = zeroRegister, rt = zeroRegister, rd = rresult, funct = Funct.OR),
                JType(addr = 2),
                IType("lhi", rs = rresult, rt = rresult, imm = 0u),
                IType("llo", rs = rresult, rt = rresult, imm = 1u)
            )
            
            ctx.LESSER() != null -> listOf(
                RType("slt", rs = 8, rt = rresult, rd = rresult, funct = Funct.SLT)
            )
            
            
            ctx.GREATER() != null -> listOf(
                RType("slt", rs = rresult, rt = 8, rd = rresult, funct = Funct.SLT)
            )
            
            
            else -> error("Unknown operation")
        }
    }
    
    override fun visitPrint(ctx : MyLangParser.PrintContext) : List<Instruction> {
        return visit(ctx.expression()) + Print()
    }
    
    override fun visitVariableExpression(ctx : MyLangParser.VariableExpressionContext) : List<Instruction> {
        val name = ctx.VARIABLE().text
        
        
        return listOf(
            IType(
                "lw",
                rs = framePointer,
                rt = rresult,
                imm = variables[name]?.toUShort() ?: error("variable '$name' is used before declaration")
            )
        )
    }
    
    override fun visitDeclaration(ctx : MyLangParser.DeclarationContext) : List<Instruction> {
        val name = ctx.VARIABLE().text
        if (variables.contains(name)) error("redeclaration of '$name'")
        
        variables[name] = variables.size * 4
        
        return push(zeroRegister)
    }
    
    override fun visitLogicalExpression(ctx : MyLangParser.LogicalExpressionContext) : List<Instruction> {
        println(ctx.children.map { it.text })
        return visit(ctx.expression(0)) + push(rresult) + visit(ctx.expression(1)) + pop(8) + when {
            ctx.AND() != null -> RType("and", rs = rresult, rt = 8, rd = rresult, funct = Funct.AND)
            ctx.OR() != null -> RType("or", rs = rresult, rt = 8, rd = rresult, funct = Funct.OR)
            else -> error("Unknown operation")
        }
    }
    
    
    override fun visitProgram(ctx : MyLangParser.ProgramContext) : List<Instruction> {
        return visit(ctx.block())
    }
    
    override fun visitWhile(ctx : MyLangParser.WhileContext) : List<Instruction> {
        val block = visit(ctx.block())
        val j = JType(addr = -(block.size + 2))
        return visit(ctx.expression()) + IType(
            "beq", rs = rresult, rt = zeroRegister, imm = (block.size + 1).toUShort()
        ) + block + j
    }
    
    override fun visitIf(ctx : MyLangParser.IfContext) : List<Instruction> {
        
        val falseBranch = (ctx.block(1)?.let { visit(it) } ?: listOf())
        val trueBranch = visit(ctx.block(0)) + JType(addr = falseBranch.size)
        
        val result = visit(ctx.expression()) + IType(
            "beq", rs = rresult, rt = zeroRegister, imm = (trueBranch.size + 1).toUShort()
        ) + trueBranch + falseBranch
        
        return result
        
    }
    
    override fun visitNotExpression(ctx : MyLangParser.NotExpressionContext) : List<Instruction> {
        return visit(ctx.getChild(1)) + RType("nor", rs = rresult, rt = zeroRegister, rd = rresult, funct = Funct.NOR)
    }
    
    override fun visitAssignment(ctx : MyLangParser.AssignmentContext) : List<Instruction> {
        val name = ctx.getChild(0).text
        
        return visit(ctx.expression()) +
                
                IType(
                    "sw",
                    rs = framePointer,
                    rt = rresult,
                    imm = variables[name]?.toUShort() ?: error("variable '$name' is assigned before declaration")
                )
        
    }
}