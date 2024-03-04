package org.example

import MyLangLexer
import MyLangParser
import org.antlr.v4.runtime.CharStreams
import org.antlr.v4.runtime.CommonTokenStream
import java.io.File

@OptIn(ExperimentalStdlibApi::class)
fun main() {
    val lexer = MyLangLexer(object {}.javaClass.getResourceAsStream("example.txt").let { CharStreams.fromStream(it) })
    
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