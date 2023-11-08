#ifndef TEMPO_COMPILER_H
#define TEMPO_COMPILER_H

#include <stdint.h>

#include "tempo/chunk.h"
#include "tempo/lexer.h"

typedef enum {
    TP_PREC_NONE,
    TP_PREC_EQUALITY,
    TP_PREC_COMPARISON,
    TP_PREC_TERM,
    TP_PREC_FACTOR,
    TP_PREC_UNARY,
    TP_PREC_PRIMARY,
} TpPrecedence;

typedef struct {
    TpLexer lexer;
    TpChunk *chunk;
    TpToken current;
    TpToken previous;
} TpCompiler;

void       tp_compiler_advance(TpCompiler *compiler);
void       tp_compiler_consume(TpCompiler *compiler, TpTokenKind kind, const char *message);
TpCompiler tp_compiler_init(const char *source, TpChunk *chunk);
bool       tp_compiler_match(TpCompiler *compiler, TpTokenKind kind);

#define tp_compile_expression(compiler) \
    tp_compile_precedence(compiler, TP_PREC_EQUALITY)

void tp_compile_binary(TpCompiler *compiler);
void tp_compile_grouping(TpCompiler *compiler);
void tp_compile_literal(TpCompiler *compiler);
void tp_compile_number(TpCompiler *compiler);
void tp_compile_precedence(TpCompiler *compiler, TpPrecedence precedence);
void tp_compile_print(TpCompiler *compiler);
void tp_compile_string(TpCompiler *compiler);
void tp_compile_unary(TpCompiler *compiler);

typedef void (*TpCompileFn)(TpCompiler *compiler);

typedef struct {
    TpCompileFn prefix;
    TpCompileFn infix;
    TpPrecedence precedence;
} TpCompileRule;

#define tp_get_compile_rule(kind) (&tp_compile_rules[kind])

extern const TpCompileRule tp_compile_rules[];

bool tp_compile_source(const char *source, TpChunk *out_chunk);
void tp_report_error(TpToken *token, const char *message);

#endif
