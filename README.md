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
statement := expression SEMICOLON;
           | PRINT expression? SEMICOLON;

expression := equality;
equality   := comparison ((BANG_EQUAL | EQUAL_EQUAL) comparison)*;
comparison := value ((GREATER | GREATER_EQUAL | LESS | LESS_EQUAL) value)*;
value      := term ((MINUS | PLUS) term)*;
term       := factor ((SLASH | STAR) factor)*;
factor     := (BANG | MINUS) factor | primary;
primary    := TRUE
            | FALSE
            | NUMBER
            | STRING
            | PAREN_LEFT expression PAREN_RIGHT;
```
