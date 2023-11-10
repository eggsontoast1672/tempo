#ifndef TEMPO_LEXER_H
#define TEMPO_LEXER_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    // Single character
    TP_TOKEN_MINUS, TP_TOKEN_PAREN_LEFT, TP_TOKEN_PAREN_RIGHT, TP_TOKEN_PLUS,
    TP_TOKEN_SEMICOLON, TP_TOKEN_SLASH, TP_TOKEN_STAR,

    // Two characters
    TP_TOKEN_BANG, TP_TOKEN_BANG_EQUAL, TP_TOKEN_EQUAL_EQUAL, TP_TOKEN_GREATER,
    TP_TOKEN_GREATER_EQUAL, TP_TOKEN_LESS, TP_TOKEN_LESS_EQUAL,

    // Arbitrary length
    TP_TOKEN_NUMBER, TP_TOKEN_STRING,

    // Keywords
    TP_TOKEN_FALSE, TP_TOKEN_PRINT, TP_TOKEN_TRUE,

    // Other
    TP_TOKEN_EOF, TP_TOKEN_ERROR,
} TpTokenKind;

typedef struct {
    TpTokenKind kind;
    const char *start;
    size_t length;
} TpToken;

typedef struct {
    const char *start;
    const char *current;
} TpLexer;

const char *tp_token_kind_to_string(TpTokenKind kind);

#define tp_lexer_done(lexer) (*lexer->current == '\0')
#define tp_lexer_peek(lexer) (*lexer->current)

char    tp_lexer_advance(TpLexer *lexer);
TpLexer tp_lexer_init(const char *source);
TpToken tp_lexer_make_error(TpLexer *lexer, const char *message);
TpToken tp_lexer_make_token(TpLexer *lexer, TpTokenKind kind);
bool    tp_lexer_match(TpLexer *lexer, char expected);
TpToken tp_lexer_next_keyword(TpLexer *lexer);
TpToken tp_lexer_next_number(TpLexer *lexer);
TpToken tp_lexer_next_string(TpLexer *lexer);
TpToken tp_lexer_next_token(TpLexer *lexer);

bool tp_is_alpha(char c);
bool tp_is_digit(char c);
bool tp_is_space(char c);

#endif
