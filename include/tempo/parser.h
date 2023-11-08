#ifndef TEMPO_PARSER_H
#define TEMPO_PARSER_H

#include <stdbool.h>

#include "tempo/chunk.h"
#include "tempo/lexer.h"

typedef struct {
    TpLexer lexer;
    TpToken current;
    TpChunk *chunk;
} TpParser;

#define tp_parser_advance(parser) (parser->current = tp_lexer_next_token(&parser->lexer))
#define tp_parser_emit_byte(parser, byte) tp_chunk_write(parser->chunk, byte)
#define tp_parser_parse_expression(parser) tp_parser_parse_equality(parser)

void tp_parser_consume(TpParser *parser, TpTokenKind kind);
void tp_parser_emit_number(TpParser *parser);
bool tp_parser_match(TpParser *parser, size_t count, ...);

void tp_parser_parse_equality(TpParser *parser);
void tp_parser_parse_comparison(TpParser *parser);
void tp_parser_parse_term(TpParser *parser);
void tp_parser_parse_factor(TpParser *parser);
void tp_parser_parse_unary(TpParser *parser);
void tp_parser_parse_primary(TpParser *parser);

#endif
