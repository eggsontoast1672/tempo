#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#define test_assert(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "[ERROR] (line %d) Expected %s to be true\n", \
                __LINE__, #expr); \
            return 1; \
        } \
    } while (0)

#endif
