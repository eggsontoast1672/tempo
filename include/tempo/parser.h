#ifndef TEMPO_PARSER_H
#define TEMPO_PARSER_H

#include <stdbool.h>

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
    TpToken current;
    TpChunk *chunk;
} TpParser;

void tp_parser_advance(TpParser *parser);
void tp_parser_consume(TpParser *parser, TpTokenKind kind, const char *message);
void tp_parser_emit_byte(TpParser *parser, uint8_t byte);
void tp_parser_emit_constant(TpParser *parser, TpValue value);
bool tp_parser_match(const TpParser *parser, size_t count, ...);
void tp_parser_report_error(TpParser *parser, const char *message);

void tp_parser_parse_expression(TpParser *parser, TpPrecedence precedence);
void tp_parser_parse_binary(TpParser *parser);
void tp_parser_parse_grouping(TpParser *parser);
void tp_parser_parse_literal(TpParser *parser);
void tp_parser_parse_number(TpParser *parser);
void tp_parser_parse_unary(TpParser *parser);

bool tp_compile_source(const char *source, TpChunk *out_chunk);

typedef struct {
    void (*prefix)(TpParser *parser);
    void (*infix)(TpParser *parser);
    TpPrecedence precedence;
} TpParseRule;

extern const TpParseRule tp_parse_rules[];

#endif
