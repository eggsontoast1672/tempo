#include "tempo/chunk.h"

#include <stdio.h>

#include "tempo/compiler.h"
#include "tempo/value.h"

void tp_chunk_free(const TpChunk *chunk) {
    free(chunk->code);
    tp_value_array_free(&chunk->constants);
}

TpChunk tp_chunk_init(void) {
    return (TpChunk) {
        .code = NULL,
        .size = 0,
        .capacity = 0,
        .constants = tp_value_array_init(),
    };
}

void tp_chunk_print(FILE *stream, const TpChunk *chunk) {
    size_t i = 0;
    for (; chunk->code[i] != TP_BYTE_RETURN;) {
        fprintf(stderr, "%04lu\t", i);
        switch (chunk->code[i]) {
        case TP_BYTE_ADD:
            fprintf(stream, "TP_BYTE_ADD\n");
            i += 1;
            break;
        case TP_BYTE_CONSTANT:
            fprintf(stream, "TP_BYTE_CONSTANT\t");
            i += 1;
            fprintf(stream, "%d\t", chunk->code[i]);
            tp_value_print(stream, chunk->constants.data[chunk->code[i]]);
            fprintf(stderr, "\n");
            i += 1;
            break;
        case TP_BYTE_DIVIDE:
            fprintf(stream, "TP_BYTE_DIVIDE\n");
            i += 1;
            break;
        case TP_BYTE_EQUAL:
            fprintf(stream, "TP_BYTE_EQUAL\n");
            i += 1;
            break;
        case TP_BYTE_FALSE:
            fprintf(stream, "TP_BYTE_FALSE\n");
            i += 1;
            break;
        case TP_BYTE_GREATER:
            fprintf(stream, "TP_BYTE_GREATER\n");
            i += 1;
            break;
        case TP_BYTE_LESS:
            fprintf(stream, "TP_BYTE_LESS\n");
            i += 1;
            break;
        case TP_BYTE_MULTIPLY:
            fprintf(stream, "TP_BYTE_MULTIPLY\n");
            i += 1;
            break;
        case TP_BYTE_NEGATE:
            fprintf(stream, "TP_BYTE_NEGATE\n");
            i += 1;
            break;
        case TP_BYTE_NOT:
            fprintf(stream, "TP_BYTE_NOT\n");
            i += 1;
            break;
        case TP_BYTE_PRINT:
            fprintf(stream, "TP_BYTE_PRINT\n");
            i += 1;
            break;
        case TP_BYTE_SUBTRACT:
            fprintf(stream, "TP_BYTE_SUBTRACT\n");
            i += 1;
            break;
        case TP_BYTE_TRUE:
            fprintf(stream, "TP_BYTE_TRUE\n");
            i += 1;
            break;
        default:
            fprintf(stream, "Unrecognized byte\n");
            i += 1;
            break;
        }
    }
    fprintf(stderr, "%04lu\t", i);
    fprintf(stream, "TP_BYTE_RETURN\n");
}

void tp_chunk_write(TpChunk *chunk, uint8_t byte) {
    if (chunk->size + 1 > chunk->capacity) {
        chunk->capacity = chunk->capacity < 8 ? 8 : chunk->capacity * 2;
        chunk->code = realloc(chunk->code, chunk->capacity * sizeof(uint8_t));
        if (chunk->code == NULL) {
            fprintf(stderr, "[ERROR] Failed to reallocate memory\n");
            exit(1);
        }
    }
    chunk->code[chunk->size] = byte;
    chunk->size += 1;
}
