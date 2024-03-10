package org.example

import java.util.*

var addressCounter = 1
    get() = field++

fun parseExpression(input: String): Triple<String, String, String>? {
    val variablePattern = """[a-zA-Z_][a-zA-Z0-9_]*"""
    val expressionPattern = """($variablePattern|\d+)\s*([><=]=?)\s*($variablePattern|\d+)"""

    val regex = Regex(expressionPattern)
    val matchResult = regex.matchEntire(input)

    return matchResult?.let {
        val a = it.groupValues[1]
        val operator = it.groupValues[2]
        val b = it.groupValues[3]

        Triple(a, b, operator)
    }
}


fun extractExpression(input: String, pattern: String): String {
    val regex = Regex("""$pattern\s*\(([^)]+)\)\s*\{""")
    val matchResult = regex.find(input)

    if (matchResult != null) {
        return matchResult.groupValues[1]
    } else {
        throw IllegalArgumentException("Строка не соответствует формату '$pattern (expression){'")
    }
}

fun parseVariableExpression(input: String): Pair<String, List<String>>? {
    val pattern = """([a-zA-Z_]+)\s*=\s*(-?\s*[a-zA-Z0-9_]+(?:\s*[+\-&|^]\s*[a-zA-Z0-9_]+){0,9})""".toRegex()
    val match = pattern.matchEntire(input.trim()) ?: return null
    val variableName = match.groupValues[1]
    val operationsString = match.groupValues[2]
    val tokens = Regex("""([+\-&|^])?\s*(-?\s*[a-zA-Z0-9_]+)""").findAll(operationsString)
    val operations = tokens.map { match ->
        "${match.groupValues[1]}${match.groupValues[2].trim()}"
    }.toList()
    if (operations.size > 10) return null
    val signedFirstOperand = if (operations.first().startsWith("-")) {
        operations.first()
    } else {
        "+${operations.first()}"
    }
    val formattedOperations = mutableListOf(signedFirstOperand)
    formattedOperations += operations.drop(1).map { it.replace(" ", "") }
    return Pair(variableName, formattedOperations)
}

fun Int.toBinaryStringPadded(): String = this.toString(2).padStart(12, '0')

fun String.toCommandSize() = this.padEnd(32, '0')


fun parse(lines: List<String>) {
    val jumps = Stack<String>()
    val builder = StringBuilder()
    val variableToAddress = mutableMapOf<String, Int>()
    val scopeVariable = mutableMapOf<Int, MutableSet<String>>()
    var currentScope = 0
    scopeVariable[currentScope] = mutableSetOf()
    lines.forEach { line ->
        val command = line.trimIndent()
        if (command.startsWith("print")) {
            val parts = command.split(" ")
            require(parts.size == 2)
            require(parts[0] == "print")
            require(currentScope in scopeVariable)
            require(scopeVariable[currentScope]!!.contains(parts[1]))
            val address = variableToAddress[parts[1]] ?: error("Unexpected error")
            builder.appendLine("1110${address.toBinaryStringPadded()}".toCommandSize())
        } else if (command.startsWith("}")) {
            require(command == "}")
            builder.append("END_SCOPE_${currentScope--}")
            builder.appendLine()
        } else if (command.startsWith("if")) {
            scopeVariable[currentScope + 1] = scopeVariable[currentScope++]!!
            val expression = extractExpression(command, "if")
            val operands = parseExpression(expression) ?: error("Unexpected expression")
            builder.append("if $operands")
            builder.appendLine()
        } else if (command.startsWith("while")) {
            scopeVariable[currentScope + 1] = scopeVariable[currentScope++]!!
            currentScope++
            val expression = extractExpression(command, "while")
            val operands = parseExpression(expression) ?: error("Unexpected expression")
            builder.append("while $operands")
            builder.appendLine()
        } else if (command.isBlank()) {
        } else {
            val expression = parseVariableExpression(command) ?: error("Unexpected command")
            val address = addressCounter
            variableToAddress.putIfAbsent(expression.first, address)
            scopeVariable[currentScope]?.add(expression.first)
            // move from the register with 0 to the temp register
            builder.appendLine("10010101000101".toCommandSize())
            expression.second.forEach {
                val sign = it.first()
                val operand = it.drop(1)
                if (operand.any { element -> element.isLetter() }) {
                    // add variable to the register
                } else {
                    // add constant to the register.
                }
            }
            builder.append(expression)
            builder.appendLine()
        }
    }
    builder.toString().also {
        println(it)
    }

}

fun main() {
    val command = """a = 10
b = a + 15 - 7
if(b>10){
print b
c = a+b
print c
a = a+b
}
print a
print b
""".trimIndent()
    parse(command.lines())
//    val expressions = listOf(
//        "b = -a + 15 - 7",
//        "b = ",
//        "x = -10",
//        "y = x * 5",
//        "z = 42 / 2",
//        "w = p & q",
//        "result = a ^ b | c & d",
//        "invalidExpr = "
//    )
//
//    for (expression in expressions) {
//        val result = parseVariableExpression(expression)
//        println("$expression: $result")
//    }
}