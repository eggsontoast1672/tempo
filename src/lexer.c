#include "tempo/lexer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char *tp_token_kind_to_string(const TpToken *token) {
    switch (token->kind) {
    case TP_TOKEN_MINUS:
        return "TP_TOKEN_MINUS";
    case TP_TOKEN_PAREN_LEFT:
        return "TP_TOKEN_PAREN_LEFT";
    case TP_TOKEN_PAREN_RIGHT:
        return "TP_TOKEN_PAREN_RIGHT";
    case TP_TOKEN_PLUS:
        return "TP_TOKEN_PLUS";
    case TP_TOKEN_SEMICOLON:
        return "TP_TOKEN_SEMICOLON";
    case TP_TOKEN_SLASH:
        return "TP_TOKEN_SLASH";
    case TP_TOKEN_STAR:
        return "TP_TOKEN_STAR";
    case TP_TOKEN_BANG:
        return "TP_TOKEN_BANG";
    case TP_TOKEN_BANG_EQUAL:
        return "TP_TOKEN_BANG_EQUAL";
    case TP_TOKEN_EQUAL_EQUAL:
        return "TP_TOKEN_EQUAL_EQUAL";
    case TP_TOKEN_GREATER:
        return "TP_TOKEN_GREATER";
    case TP_TOKEN_GREATER_EQUAL:
        return "TP_TOKEN_GREATER_EQUAL";
    case TP_TOKEN_LESS:
        return "TP_TOKEN_LESS";
    case TP_TOKEN_LESS_EQUAL:
        return "TP_TOKEN_LESS_EQUAL";
    case TP_TOKEN_NUMBER:
        return "TP_TOKEN_NUMBER";
    case TP_TOKEN_STRING:
        return "TP_TOKEN_STRING";
    case TP_TOKEN_FALSE:
        return "TP_TOKEN_FALSE";
    case TP_TOKEN_PRINT:
        return "TP_TOKEN_PRINT";
    case TP_TOKEN_TRUE:
        return "TP_TOKEN_TRUE";
    case TP_TOKEN_EOF:
        return "TP_TOKEN_EOF";
    case TP_TOKEN_ERROR:
        return "TP_TOKEN_ERROR";
    }
}

char tp_lexer_advance(TpLexer *lexer) {
    if (tp_lexer_done(lexer)) {
        return '\0';
    }
    return *lexer->current++;
}

TpLexer tp_lexer_init(const char *source) {
    return (TpLexer) {
        .start = source,
        .current = source,
    };
}

TpToken tp_lexer_make_error(TpLexer *lexer, const char *message) {
    return (TpToken) {
        .kind = TP_TOKEN_ERROR,
        .start = message,
        .length = strlen(message),
    };
}

TpToken tp_lexer_make_token(TpLexer *lexer, TpTokenKind kind) {
    return (TpToken) {
        .kind = kind,
        .start = lexer->start,
        .length = lexer->current - lexer->start,
    };
}

bool tp_lexer_match(TpLexer *lexer, char expected) {
    if (*lexer->current == '\0' || *lexer->current != expected) {
        return false;
    }
    ++lexer->current;
    return true;
}

TpToken tp_lexer_next_keyword(TpLexer *lexer) {
    while (tp_is_alpha(tp_lexer_peek(lexer))) {
        tp_lexer_advance(lexer);
    }

    // "print 1 + 2;"
    //  ^    ^

    size_t length = lexer->current - lexer->start;
    if (strncmp(lexer->start, "false", length) == 0) {
        return tp_lexer_make_token(lexer, TP_TOKEN_FALSE);
    }
    if (strncmp(lexer->start, "print", length) == 0) {
        return tp_lexer_make_token(lexer, TP_TOKEN_PRINT);
    }
    if (strncmp(lexer->start, "true", length) == 0) {
        return tp_lexer_make_token(lexer, TP_TOKEN_TRUE);
    }
    return tp_lexer_make_error(lexer, "invalid keyword");
}

TpToken tp_lexer_next_number(TpLexer *lexer) {
    while (tp_is_digit(tp_lexer_peek(lexer))) {
        tp_lexer_advance(lexer);
    }
    if (tp_lexer_peek(lexer) == '.') {
        tp_lexer_advance(lexer);
        while (tp_is_digit(tp_lexer_peek(lexer))) {
            tp_lexer_advance(lexer);
        }
    }
    return tp_lexer_make_token(lexer, TP_TOKEN_NUMBER);
}

TpToken tp_lexer_next_string(TpLexer *lexer) {
    while (!tp_lexer_done(lexer) && tp_lexer_advance(lexer) != '"');
    if (tp_lexer_done(lexer)) {
        return tp_lexer_make_error(lexer, "unterminated string");
    }
    return tp_lexer_make_token(lexer, TP_TOKEN_STRING);
}

TpToken tp_lexer_next_token(TpLexer *lexer) {
    // TODO: Handle the following:
    // - TP_TOKEN_EQUAL_EQUAL
    // - TP_TOKEN_FALSE
    // - TP_TOKEN_TRUE
    while (tp_is_space(tp_lexer_peek(lexer))) {
        tp_lexer_advance(lexer);
    }
    lexer->start = lexer->current;
    if (tp_lexer_done(lexer)) {
        return tp_lexer_make_token(lexer, TP_TOKEN_EOF);
    }

    char c = tp_lexer_advance(lexer);
    if (tp_is_alpha(c)) {
        return tp_lexer_next_keyword(lexer);
    }
    if (tp_is_digit(c)) {
        return tp_lexer_next_number(lexer);
    }
    switch (c) {
    case '-':
        return tp_lexer_make_token(lexer, TP_TOKEN_MINUS);
    case '(':
        return tp_lexer_make_token(lexer, TP_TOKEN_PAREN_LEFT);
    case ')':
        return tp_lexer_make_token(lexer, TP_TOKEN_PAREN_RIGHT);
    case '+':
        return tp_lexer_make_token(lexer, TP_TOKEN_PLUS);
    case ';':
        return tp_lexer_make_token(lexer, TP_TOKEN_SEMICOLON);
    case '/':
        return tp_lexer_make_token(lexer, TP_TOKEN_SLASH);
    case '*':
        return tp_lexer_make_token(lexer, TP_TOKEN_STAR);
    case '!':
        return tp_lexer_make_token(lexer, tp_lexer_match(lexer, '=')
                                   ? TP_TOKEN_BANG_EQUAL
                                   : TP_TOKEN_BANG);
    case '=':
        if (tp_lexer_match(lexer, '=')) {
            return tp_lexer_make_token(lexer, TP_TOKEN_EQUAL_EQUAL);
        }
        break;
    case '>':
        return tp_lexer_make_token(lexer, tp_lexer_match(lexer, '=')
                                   ? TP_TOKEN_GREATER_EQUAL
                                   : TP_TOKEN_GREATER);
    case '<':
        return tp_lexer_make_token(lexer, tp_lexer_match(lexer, '=')
                                   ? TP_TOKEN_LESS_EQUAL
                                   : TP_TOKEN_LESS);
    case '"':
        return tp_lexer_next_string(lexer);
    }
    return tp_lexer_make_error(lexer, "unrecognized token");
}

bool tp_is_alpha(char c) {
    return 'A' <= c && c <= 'Z' ||
           'a' <= c && c <= 'z' ||
           c == '_';
}

bool tp_is_digit(char c) {
    return '0' <= c && c <= '9';
}

bool tp_is_space(char c) {
    return c == '\t' ||
           c == '\n' ||
           c == '\r' ||
           c == ' ';
}
