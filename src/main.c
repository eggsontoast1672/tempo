#include <stdbool.h>
#include <stdio.h>

#include "tempo/chunk.h"
#include "tempo/lexer.h"
#include "tempo/parser.h"

static char *read_file_to_string(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, );
    }
}

static void print_program_tokens(const char *source) {
    TpLexer lexer = tp_lexer_init(source);
    while (true) {
        TpToken token = tp_lexer_next_token(&lexer);
        fprintf(stderr, "%s\n", tp_token_kind_to_string(token.kind));
        if (token.kind == TP_TOKEN_EOF) {
            break;
        }
    }
}

static void test_program(const char *source) {
    TpChunk chunk = tp_chunk_init();
    tp_compile_source(source, &chunk);
    tp_chunk_print(stderr, &chunk);
    tp_chunk_free(&chunk);
}

static void run_file(const char *path) {

}

static void run_test_programs(void) {
    test_program("print 1 + 2;");
    test_program("print !((true != false) == false);");
    test_program("print 1.8 > 9.5;");
    test_program("print 4.99999 <= -9.5;");
    test_program("print 70.0 < 9.50000002;");
    test_program("print 0.42 <= 323232323.10101;");
    test_program("print 1 + 3 * 7 - 10 / 5;");
}

int main(void) {
    char line[1024];
    while (true) {
        printf("> ");
        if (!fgets(line, sizeof line, stdin)) {
            printf("\n");
            return 0;
        }
        test_program(line);
    }
    return 0;
}
