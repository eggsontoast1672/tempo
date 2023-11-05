#ifndef TEMPO_COMPILER_H
#define TEMPO_COMPILER_H

#include <stdint.h>

#include "tempo/lexer.h"

typedef struct {
    TpToken current;
    TpToken previous;
    TpLexer lexer;
} TpParser;

typedef struct Chunk Chunk;

void tp_parser_advance(TpParser *parser);
void tp_parser_consume(TpParser *parser, TpTokenKind kind, const char *message);

void tp_compile_source(const char *source);
void tp_report_error(TpToken *token, const char *message);

#endif
