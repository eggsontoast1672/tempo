# Tempo

Types:

- Booleans
- Numbers (64-bit floating point)
- Strings

Unary Operators:

- Boolean Negation: !
- Numeric Negation: -

Binary Operators:

- Arithmetic: + - * /   (numbers only)
- Comparison: > >= < <= (numbers only)
- Equality: != ==       (operands must be isotypical)

Grammar:

```
program   := statement* EOF;
statement := PRINT expression SEMICOLON;
           | expression SEMICOLON;

expression := equality;
equality   := comparison ((BANG_EQUAL | EQUAL_EQUAL) comparison)*;
comparison := term ((GREATER | GREATER_EQUAL | LESS | LESS_EQUAL) term)*;
term       := factor ((MINUS | PLUS) factor)*;
factor     := unary ((SLASH | STAR) unary)*;
unary      := (BANG | MINUS) unary | primary;
primary    := TRUE
            | FALSE
            | NUMBER
            | STRING
            | PAREN_LEFT expression PAREN_RIGHT;
```

```
=== EXPRESSION ===
true <eof>
^~~~ ^~~~~

=== CALL STACK ===
tp_compile_precedence(TP_PREC_EQUALITY);

===  BYTECODE  ===
TP_BYTE_TRUE
```
