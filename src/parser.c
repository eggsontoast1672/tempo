#include "tempo/parser.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "tempo/chunk.h"
#include "tempo/lexer.h"
#include "tempo/value.h"

void tp_parser_consume(TpParser *parser, TpTokenKind kind) {
    if (parser->current.kind != kind) {
        fprintf(stderr, "[ERROR] expected %d, got %d\n",
                kind, parser->current.kind);
    } else {
        tp_parser_advance(parser);
    }
}

void tp_parser_emit_number(TpParser *parser) {
    double value = strtod(parser->current.start, NULL);
    size_t index = tp_chunk_write_constant(parser->chunk, (TpValue) {
        .type = TP_VALUE_NUMBER,
        .as_number = value,
    });
    if (index > UINT8_MAX) {
        fprintf(stderr, "[ERROR] Too many constants in chunk %p\n",
                parser->chunk);
        return;
    }
    tp_parser_emit_byte(parser, index);
}

bool tp_parser_match(TpParser *parser, size_t count, ...) {
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

void tp_parser_parse_equality(TpParser *parser) {
    tp_parser_parse_comparison(parser);

    if (tp_parser_match(parser, 2, TP_TOKEN_BANG_EQUAL, TP_TOKEN_EQUAL_EQUAL)) {
        TpTokenKind kind = parser->current.kind;
        tp_parser_advance(parser);
        tp_parser_parse_comparison(parser);

        if (kind == TP_TOKEN_BANG_EQUAL) {
            tp_parser_emit_byte(parser, TP_BYTE_EQUAL);
            tp_parser_emit_byte(parser, TP_BYTE_NOT);
        } else if (kind == TP_TOKEN_EQUAL_EQUAL) {
            tp_parser_emit_byte(parser, TP_BYTE_EQUAL);
        }
    }
}

void tp_parser_parse_comparison(TpParser *parser) {
    tp_parser_parse_term(parser);

    if (tp_parser_match(parser, 4,
                        TP_TOKEN_GREATER,
                        TP_TOKEN_GREATER_EQUAL,
                        TP_TOKEN_LESS,
                        TP_TOKEN_LESS_EQUAL))
    {
        TpTokenKind kind = parser->current.kind;
        tp_parser_advance(parser);
        tp_parser_parse_term(parser);

        switch (kind) {
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
}

void tp_parser_parse_term(TpParser *parser) {
    tp_parser_parse_factor(parser);

    if (tp_parser_match(parser, 2, TP_TOKEN_MINUS, TP_TOKEN_PLUS)) {
        TpTokenKind kind = parser->current.kind;
        tp_parser_advance(parser);
        tp_parser_parse_factor(parser);

        switch (kind) {
        case TP_TOKEN_MINUS:
            tp_parser_emit_byte(parser, TP_BYTE_SUBTRACT);
            break;
        case TP_TOKEN_PLUS:
            tp_parser_emit_byte(parser, TP_BYTE_ADD);
            break;
        default:
            break;
        }
    }
}

void tp_parser_parse_factor(TpParser *parser) {
    tp_parser_parse_unary(parser);

    if (tp_parser_match(parser, 2, TP_TOKEN_SLASH, TP_TOKEN_STAR)) {
        TpTokenKind kind = parser->current.kind;
        tp_parser_advance(parser);
        tp_parser_parse_unary(parser);

        switch (kind) {
        case TP_TOKEN_SLASH:
            tp_parser_emit_byte(parser, TP_BYTE_DIVIDE);
            break;
        case TP_TOKEN_STAR:
            tp_parser_emit_byte(parser, TP_BYTE_MULTIPLY);
            break;
        default:
            break;
        }
    }
}

void tp_parser_parse_unary(TpParser *parser) {
    if (tp_parser_match(parser, 2, TP_TOKEN_BANG, TP_TOKEN_MINUS)) {
        TpTokenKind kind = parser->current.kind;
        tp_parser_parse_unary(parser);

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
    } else {
        tp_parser_parse_primary(parser);
    }
}

void tp_parser_parse_primary(TpParser *parser) {
    switch (parser->current.kind) {
    case TP_TOKEN_TRUE:
        tp_parser_emit_byte(parser, TP_BYTE_TRUE);
        tp_parser_advance(parser);
        break;
    case TP_TOKEN_FALSE:
        tp_parser_emit_byte(parser, TP_BYTE_FALSE);
        tp_parser_advance(parser);
        break;
    case TP_TOKEN_NUMBER:
        tp_parser_emit_number(parser);
        tp_parser_advance(parser);
        break;
    case TP_TOKEN_PAREN_LEFT:
        tp_parser_advance(parser);
        tp_parser_parse_expression(parser);
        tp_parser_consume(parser, TP_TOKEN_PAREN_RIGHT);
        break;
    default:
        break;
    }
}
