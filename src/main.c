#include <stdbool.h>
#include <stdio.h>

#include "tempo/lexer.h"

static void test_program(const char *name, const char *source) {
    if (name != NULL) {
        printf("\n== %s ==\n", name);
    }
    TpLexer lexer = tp_lexer_init(source);
    TpToken token;
    while ((token = tp_lexer_next_token(&lexer)).kind != TP_TOKEN_EOF) {
        printf("%s (%.*s)\n",
               tp_token_kind_to_string(&token),
               (int)token.length,
               token.start);
    }
}

static void run_test_programs(void) {
    test_program("add two numbers", "print 1 + 2;");
    test_program("evaluate logical expression", "print !((true != false) == false);");
    test_program("compare two numbers (1)", "print 1.8 > 9.5;");
    test_program("compare two numbers (2)", "print 4.99999 <= -9.5;");
    test_program("compare two numbers (3)", "print 70.0 < 9.50000002;");
    test_program("compare two numbers (4)", "print 0.42 <= 323232323.10101;");
    test_program("more math", "print 1 + 3 * 7 - 10 / 5;");
}

int main(void) {
    char line[1024];
    while (true) {
        printf("> ");
        if (!fgets(line, sizeof line, stdin)) {
            printf("\n");
            return 0;
        }
        test_program(NULL, line);
    }
    return 0;
}
