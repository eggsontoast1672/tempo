#include "tempo/compiler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tempo/chunk.h"
#include "tempo/lexer.h"

// TP_PREC_NONE       = 0
// TP_PREC_EQUALITY   = 1
// TP_PREC_COMPARISON = 2
// TP_PREC_TERM       = 3
// TP_PREC_FACTOR     = 4
// TP_PREC_UNARY      = 5
// TP_PREC_PRIMARY    = 6

void tp_compiler_advance(TpCompiler *compiler) {
    compiler->previous = compiler->current;
    compiler->current = tp_lexer_next_token(&compiler->lexer);
}

void tp_compiler_consume(TpCompiler *compiler, TpTokenKind kind, const char *message) {
    if (compiler->current.kind == kind) {
        tp_compiler_advance(compiler);
        return;
    }
    tp_report_error(&compiler->current, message);
}

TpCompiler tp_compiler_init(const char *source, TpChunk *chunk) {
    return (TpCompiler) {
        .lexer = tp_lexer_init(source),
        .chunk = chunk,
        .previous = (TpToken) {
            .kind = TP_TOKEN_EOF,
            .start = "",
            .length = 0,
        },
        .current = (TpToken) {
            .kind = TP_TOKEN_EOF,
            .start = "",
            .length = 0,
        },
    };
}

bool tp_compiler_match(TpCompiler *compiler, TpTokenKind kind) {
    if (compiler->current.kind == kind) {
        tp_compiler_advance(compiler);
        return true;
    }
    return false;
}

const TpCompileRule tp_compile_rules[] = {
    [TP_TOKEN_MINUS]         = {tp_compile_unary,    tp_compile_binary, TP_PREC_TERM},
    [TP_TOKEN_PAREN_LEFT]    = {tp_compile_grouping, NULL,              TP_PREC_NONE},
    [TP_TOKEN_PAREN_RIGHT]   = {NULL,                NULL,              TP_PREC_NONE},
    [TP_TOKEN_PLUS]          = {NULL,                tp_compile_binary, TP_PREC_TERM},
    [TP_TOKEN_SEMICOLON]     = {NULL,                NULL,              TP_PREC_NONE},
    [TP_TOKEN_SLASH]         = {NULL,                tp_compile_binary, TP_PREC_FACTOR},
    [TP_TOKEN_STAR]          = {NULL,                tp_compile_binary, TP_PREC_FACTOR},
    [TP_TOKEN_BANG]          = {tp_compile_unary,    NULL,              TP_PREC_NONE},
    [TP_TOKEN_BANG_EQUAL]    = {NULL,                tp_compile_binary, TP_PREC_EQUALITY},
    [TP_TOKEN_EQUAL_EQUAL]   = {NULL,                tp_compile_binary, TP_PREC_EQUALITY},
    [TP_TOKEN_GREATER]       = {NULL,                tp_compile_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_GREATER_EQUAL] = {NULL,                tp_compile_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_LESS]          = {NULL,                tp_compile_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_LESS_EQUAL]    = {NULL,                tp_compile_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_NUMBER]        = {tp_compile_number,   NULL,              TP_PREC_NONE},
    [TP_TOKEN_STRING]        = {tp_compile_string,   NULL,              TP_PREC_NONE},
    [TP_TOKEN_FALSE]         = {tp_compile_literal,  NULL,              TP_PREC_NONE},
    [TP_TOKEN_PRINT]         = {NULL,                NULL,              TP_PREC_NONE},
    [TP_TOKEN_TRUE]          = {tp_compile_literal,  NULL,              TP_PREC_NONE},
    [TP_TOKEN_EOF]           = {NULL,                NULL,              TP_PREC_NONE},
    [TP_TOKEN_ERROR]         = {NULL,                NULL,              TP_PREC_NONE},
};

void tp_compile_binary(TpCompiler *compiler) {
    TpTokenKind operator_kind = compiler->previous.kind;
    const TpCompileRule *rule = tp_get_compile_rule(operator_kind);
    tp_compile_precedence(compiler, rule->precedence + 1);
    switch (operator_kind) {
    case TP_TOKEN_MINUS:
        tp_chunk_write(compiler->chunk, TP_BYTE_SUBTRACT);
        break;
    case TP_TOKEN_PLUS:
        tp_chunk_write(compiler->chunk, TP_BYTE_ADD);
        break;
    case TP_TOKEN_SLASH:
        tp_chunk_write(compiler->chunk, TP_BYTE_DIVIDE);
        break;
    case TP_TOKEN_STAR:
        tp_chunk_write(compiler->chunk, TP_BYTE_MULTIPLY);
        break;
    case TP_TOKEN_BANG_EQUAL:
        tp_chunk_write(compiler->chunk, TP_BYTE_EQUAL);
        tp_chunk_write(compiler->chunk, TP_BYTE_NOT);
        break;
    case TP_TOKEN_EQUAL_EQUAL:
        tp_chunk_write(compiler->chunk, TP_BYTE_EQUAL);
        break;
    case TP_TOKEN_GREATER:
        tp_chunk_write(compiler->chunk, TP_BYTE_GREATER);
        break;
    case TP_TOKEN_GREATER_EQUAL:
        tp_chunk_write(compiler->chunk, TP_BYTE_LESS);
        tp_chunk_write(compiler->chunk, TP_BYTE_NOT);
        break;
    case TP_TOKEN_LESS:
        tp_chunk_write(compiler->chunk, TP_BYTE_LESS);
        break;
    case TP_TOKEN_LESS_EQUAL:
        tp_chunk_write(compiler->chunk, TP_BYTE_GREATER);
        tp_chunk_write(compiler->chunk, TP_BYTE_NOT);
        break;
    default:
        assert(false); // unreachable
        break;
    }
}

void tp_compile_grouping(TpCompiler *compiler) {
    tp_compile_expression(compiler);
    tp_compiler_consume(compiler,
                        TP_TOKEN_PAREN_RIGHT,
                        "Expect ')' after expression");
}

void tp_compile_literal(TpCompiler *compiler) {
    switch (compiler->previous.kind) {
    case TP_TOKEN_FALSE:
        tp_chunk_write(compiler->chunk, TP_BYTE_FALSE);
        break;
    case TP_TOKEN_TRUE:
        tp_chunk_write(compiler->chunk, TP_BYTE_TRUE);
        break;
    default:
        assert(false); // unreachable
        break;
    }
}

void tp_compile_number(TpCompiler *compiler) {
    double value = strtod(compiler->previous.start, NULL);
    tp_value_array_write(&compiler->chunk->constants, (TpValue) {
        .type = TP_VALUE_NUMBER,
        .as_number = value,
    });
    tp_chunk_write(compiler->chunk, TP_BYTE_CONSTANT);
    tp_chunk_write(compiler->chunk, compiler->chunk->constants.size - 1);
}

void tp_compile_precedence(TpCompiler *compiler, TpPrecedence precedence) {
    tp_compiler_advance(compiler);
    TpCompileFn prefix_rule = tp_get_compile_rule(compiler->previous.kind)->prefix;
    if (prefix_rule == NULL) {
        tp_report_error(&compiler->previous, "Expect expression");
        return;
    }
    prefix_rule(compiler);
    while (precedence <= tp_get_compile_rule(compiler->current.kind)->precedence) {
        tp_compiler_advance(compiler);
        tp_get_compile_rule(compiler->previous.kind)->infix(compiler);
    }
}

void tp_compile_print(TpCompiler *compiler) {
    tp_compiler_advance(compiler);
    tp_compile_expression(compiler);
    tp_compiler_consume(compiler,
                        TP_TOKEN_SEMICOLON,
                        "Expect ';' after expression");
    tp_chunk_write(compiler->chunk, TP_BYTE_PRINT);
}

void tp_compile_unary(TpCompiler *compiler) {
    TpTokenKind operator_kind = compiler->previous.kind;
    tp_compile_precedence(compiler, TP_PREC_UNARY);
    switch (operator_kind) {
    case TP_TOKEN_MINUS:
        tp_chunk_write(compiler->chunk, TP_BYTE_NEGATE);
        break;
    case TP_TOKEN_BANG:
        tp_chunk_write(compiler->chunk, TP_BYTE_NOT);
        break;
    default:
        assert(false); // unreachable
        break;
    }
}

void tp_compile_string(TpCompiler *compiler) {
    assert(false && "TODO: Implement tp_compile_string");
}

bool tp_compile_source(const char *source, TpChunk *out_chunk) {
    TpCompiler compiler = tp_compiler_init(source, out_chunk);
    tp_compiler_advance(&compiler);
    while (compiler.current.kind != TP_TOKEN_EOF) {
        if (compiler.current.kind == TP_TOKEN_PRINT) {
            tp_compile_print(&compiler);
        } else {
            tp_compile_expression(&compiler);
        }
    }
    tp_chunk_write(compiler.chunk, TP_BYTE_RETURN);
    return true;
}

void tp_report_error(TpToken *token, const char *message) {
    fprintf(stderr, "Error");
    if (token->kind == TP_TOKEN_EOF) {
        fprintf(stderr, " at end of file");
    } else if (token->kind != TP_TOKEN_ERROR) {
        fprintf(stderr, " near '%.*s'", (int)token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
}
