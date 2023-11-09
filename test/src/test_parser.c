#include "tempo/lexer.h"
#include "tempo/parser.h"

#include "test_utils.h"

static int test_parser_match(void) {
    TpParser parser = (TpParser) {
        .current = TP_TOKEN_PLUS,
    };
    test_assert(tp_parser_match(&parser, 1, TP_TOKEN_PLUS));
    test_assert(!tp_parser_match(&parser, 1, TP_TOKEN_MINUS));
    return 0;
}

int main(void) {
    test_assert(test_parser_match() == 0);
    return 0;
}
