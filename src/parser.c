#include "tempo/compiler.h"

#include <stdio.h>

#include "tempo/lexer.h"

void tp_parser_advance(TpParser *parser) {
    parser->previous = parser->current;
    while (true) {
        parser->current = tp_lexer_next_token(&parser->lexer);
        if (parser->current.kind != TP_TOKEN_ERROR) {
            break;
        }
        tp_report_error(&parser->current, parser->current.start);
    }
}

void tp_parser_consume(TpParser *parser, TpTokenKind kind, const char *message) {
    if (parser->current.kind == kind) {
        tp_parser_advance(parser);
    } else {
        tp_report_error(&parser->current, message);
    }
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
