#ifndef TEMPO_CHUNK_H
#define TEMPO_CHUNK_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "tempo/value.h"

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

#endif
