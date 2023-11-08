#ifndef TEMPO_CHUNK_H
#define TEMPO_CHUNK_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "tempo/value.h"

typedef enum {
    TP_BYTE_ADD,
    TP_BYTE_CONSTANT,
    TP_BYTE_DIVIDE,
    TP_BYTE_EQUAL,
    TP_BYTE_FALSE,
    TP_BYTE_GREATER,
    TP_BYTE_LESS,
    TP_BYTE_MULTIPLY,
    TP_BYTE_NEGATE,
    TP_BYTE_NOT,
    TP_BYTE_PRINT,
    TP_BYTE_RETURN,
    TP_BYTE_SUBTRACT,
    TP_BYTE_TRUE,
} TpByte;

typedef struct {
    uint8_t     *code;
    size_t       size;
    size_t       capacity;
    TpValueArray constants;
} TpChunk;

void    tp_chunk_free(const TpChunk *chunk);
TpChunk tp_chunk_init(void);
void    tp_chunk_print(FILE *stream, const TpChunk *chunk);
void    tp_chunk_write(TpChunk *chunk, uint8_t byte);
size_t  tp_chunk_write_constant(TpChunk *chunk, TpValue value);

#endif
