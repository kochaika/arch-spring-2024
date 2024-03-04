package org.example

import MyLangLexer
import MyLangParser
import org.antlr.v4.runtime.CharStreams
import org.antlr.v4.runtime.CommonTokenStream
import java.io.File
import kotlin.system.exitProcess

fun main(args : Array<String>) {
    if (args.isEmpty()) {
        println("No input file provided")
        exitProcess(1)
    }
    
    val lexer = MyLangLexer(CharStreams.fromFileName(args[0]))
    
    val tokens = CommonTokenStream(lexer)
    val parser = MyLangParser(tokens)
    val program = parser.program()
    
    val compiler = MyLangCompiler()
    
    val result = compiler.visit(program)
    val binary = result.map { it.toInt() }
    
    result.forEach { println(it.toBinaryString()) }
    val byteArray = ByteArray(binary.size * 4) { i ->
        val pos = i / 4
        val shift = (3 - i % 4) * 8
        (binary[pos] shr shift).toByte()
    }
    val file = File("a.out")
    
    
    file.writeBytes(byteArray)
    
    File("a.txt").writeText(result.joinToString(" ") { it.toBinaryString() })
    println(result)
    
}