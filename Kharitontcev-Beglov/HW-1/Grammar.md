# Grammar for Klang (KhB's language)

## Program

Program — is a block of statements, defined in the file.

## Block

```
BLOCK := { <STMTS> }
<STMTS> := <Statement> ((EOL | ;) <STMTS>)?
```

Block — the list of statemented split by newline or semicolon delimited by parentheses.

## Statements

### Variable declaration (DEPRECATED, default values of every variable = 0)

```
VAR_DECL := var <VAR_LIST>
VAR_LIST := <VAR_NAME> (,<VAR_LIST>)?
```

Declares the list of distinctive variables.
Throws a panic, if redeclaration in the current block occurs.
User of a variable without defining it is not allowed.

### Variable assignment

```
VAR_ASSIGN := <VAR_NAME> = <EXPR>
```

Assigns the value into the defined variable.

### If

```
IF := if (<EXPR>) <BLOCK> (else <BLOCK>)?
```

Just if-else conditional branch. If the expr is not zero, then goes to the first (true) branch, otherwise if the false
branch exists goes to it.

### While

```
WHILE := while (<EXPR>) <BLOCK>
```
Just while. Run if condition is not zero.
### Print
```
PRINT := print (<expr>)
```
Prints the output into the stdout.

## Expressions
### Variable name
```
VAR_NAME := [a-zA-Z]+
```
only letters.
### Integer literal
```
LITERAL := [0-9] | 1[0-9]+
```
### Operations
```
OP := + | - | & | "|" |
```
Operations: + — plus, - — minus, & — bitwise and, | — bitwise or, ~ — bitwise not
### Atomic expression
```
ATOMIC := <VAR_NAME> | <LITERAL> | ~ <EXPR>
```
### Expression Grammar
```
EXPR := (<EXPR> <OP> <EXPR>) | <ATOMIC> | "(" <EXPR> ")"
```