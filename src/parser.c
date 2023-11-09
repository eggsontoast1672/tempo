#include "tempo/parser.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "tempo/chunk.h"
#include "tempo/lexer.h"
#include "tempo/value.h"

void tp_parser_advance(TpParser *parser) {
    parser->current = tp_lexer_next_token(&parser->lexer);
}

void tp_parser_consume(TpParser *parser, TpTokenKind kind, const char *message) {
    if (parser->current.kind != kind) {
        tp_parser_report_error(parser);
    }
    tp_parser_advance(parser);
}

void tp_parser_emit_byte(TpParser *parser, uint8_t byte) {
    tp_chunk_write(parser->chunk, byte);
}

void tp_parser_emit_constant(TpParser *parser, TpValue value) {
    size_t index = tp_chunk_write_constant(parser->chunk, value);
    if (index > UINT8_MAX) {
        tp_parser_report_error(parser, "Too many constants in chunk");
        return;
    }
    tp_parser_emit_byte(parser, index);
}

bool tp_parser_match(const TpParser *parser, size_t count, ...) {
    va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        if (parser->current.kind == va_arg(args, TpTokenKind)) {
            return true;
        }
    }
    va_end(args);
    return false;
}

void tp_parser_parse_binary(TpParser *parser) {
    TpTokenKind kind = parser->current.kind;
    tp_parser_advance(parser);
    tp_parser_parse_expression(parser, tp_parse_rules[kind].precedence + 1);
    switch (kind) {
    case TP_TOKEN_MINUS:
        tp_parser_emit_byte(parser, TP_BYTE_SUBTRACT);
        break;
    case TP_TOKEN_PLUS:
        tp_parser_emit_byte(parser, TP_BYTE_ADD);
        break;
    case TP_TOKEN_SLASH:
        tp_parser_emit_byte(parser, TP_BYTE_DIVIDE);
        break;
    case TP_TOKEN_STAR:
        tp_parser_emit_byte(parser, TP_BYTE_MULTIPLY);
        break;
    case TP_TOKEN_BANG_EQUAL:
        tp_parser_emit_byte(parser, TP_BYTE_EQUAL);
        tp_parser_emit_byte(parser, TP_BYTE_NOT);
        break;
    case TP_TOKEN_EQUAL_EQUAL:
        tp_parser_emit_byte(parser, TP_BYTE_EQUAL);
        break;
    case TP_TOKEN_GREATER:
        tp_parser_emit_byte(parser, TP_BYTE_GREATER);
        break;
    case TP_TOKEN_GREATER_EQUAL:
        tp_parser_emit_byte(parser, TP_BYTE_LESS);
        tp_parser_emit_byte(parser, TP_BYTE_NOT);
        break;
    case TP_TOKEN_LESS:
        tp_parser_emit_byte(parser, TP_BYTE_LESS);
        break;
    case TP_TOKEN_LESS_EQUAL:
        tp_parser_emit_byte(parser, TP_BYTE_GREATER);
        tp_parser_emit_byte(parser, TP_BYTE_NOT);
        break;
    default:
        break;
    }
}

void tp_parser_parse_grouping(TpParser *parser) {
    tp_parser_advance(parser);
    tp_parser_parse_expression(parser, TP_PREC_EQUALITY);
    tp_parser_consume(parser, TP_TOKEN_PAREN_RIGHT,
                      "Expected ')' after expression");
}

void tp_parser_parse_literal(TpParser *parser) {
    TpTokenKind kind = parser->current.kind;
    tp_parser_advance(parser);
    switch (kind) {
    case TP_TOKEN_FALSE:
        tp_parser_emit_byte(parser, TP_BYTE_FALSE);
        break;
    case TP_TOKEN_TRUE:
        tp_parser_emit_byte(parser, TP_BYTE_TRUE);
        break;
    default:
        break;
    }
}

void tp_parser_parse_number(TpParser *parser) {
    double value = strtod(parser->current.start, NULL);
    tp_parser_advance(parser);
    tp_parser_emit_constant(parser, (TpValue) {
        .type = TP_VALUE_NUMBER,
        .as_number = value,
    });
}

void tp_parser_parse_unary(TpParser *parser) {
    TpTokenKind kind = parser->current.kind;
    tp_parser_advance(parser);
    tp_parser_parse_expression(parser, TP_PREC_UNARY);
    switch (kind) {
    case TP_TOKEN_BANG:
        tp_parser_emit_byte(parser, TP_BYTE_NOT);
        break;
    case TP_TOKEN_MINUS:
        tp_parser_emit_byte(parser, TP_BYTE_NEGATE);
        break;
    default:
        break;
    }
}

bool tp_compile_source(const char *source, TpChunk *out_chunk) {
    TpParser parser = {
        .lexer = tp_lexer_init(source),
        .current = (TpToken) {
            .kind = TP_TOKEN_EOF,
            .start = "",
            .length = 0,
        },
        .chunk = out_chunk,
    };
    tp_parser_advance(&parser);
    tp_parser_parse_expression(&parser, TP_PREC_EQUALITY);
    tp_parser_emit_byte(&parser, TP_BYTE_RETURN);
    return true;
}

const TpParseRule tp_parse_rules[] = {
    [TP_TOKEN_BANG]          = {tp_parser_parse_unary,    NULL,                   TP_PREC_NONE},
    [TP_TOKEN_BANG_EQUAL]    = {NULL,                     tp_parser_parse_binary, TP_PREC_EQUALITY},
    [TP_TOKEN_EOF]           = {NULL,                     NULL,                   TP_PREC_NONE},
    [TP_TOKEN_EQUAL_EQUAL]   = {NULL,                     tp_parser_parse_binary, TP_PREC_EQUALITY},
    [TP_TOKEN_ERROR]         = {NULL,                     NULL,                   TP_PREC_NONE},
    [TP_TOKEN_FALSE]         = {tp_parser_parse_literal,  NULL,                   TP_PREC_NONE},
    [TP_TOKEN_GREATER]       = {NULL,                     tp_parser_parse_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_GREATER_EQUAL] = {NULL,                     tp_parser_parse_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_LESS]          = {NULL,                     tp_parser_parse_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_LESS_EQUAL]    = {NULL,                     tp_parser_parse_binary, TP_PREC_COMPARISON},
    [TP_TOKEN_MINUS]         = {tp_parser_parse_unary,    tp_parser_parse_binary, TP_PREC_TERM},
    [TP_TOKEN_NUMBER]        = {tp_parser_parse_number,   NULL,                   TP_PREC_NONE},
    [TP_TOKEN_PAREN_LEFT]    = {tp_parser_parse_grouping, NULL,                   TP_PREC_NONE},
    [TP_TOKEN_PAREN_RIGHT]   = {NULL,                     NULL,                   TP_PREC_NONE},
    [TP_TOKEN_PLUS]          = {NULL,                     tp_parser_parse_binary, TP_PREC_TERM},
    [TP_TOKEN_SEMICOLON]     = {NULL,                     NULL,                   TP_PREC_NONE},
    [TP_TOKEN_SLASH]         = {NULL,                     tp_parser_parse_binary, TP_PREC_FACTOR},
    [TP_TOKEN_STAR]          = {NULL,                     tp_parser_parse_binary, TP_PREC_FACTOR},
    [TP_TOKEN_STRING]        = {NULL,                     NULL,                   TP_PREC_NONE},
    [TP_TOKEN_TRUE]          = {tp_parser_parse_literal,  NULL,                   TP_PREC_NONE},
};
