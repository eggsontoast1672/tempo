#ifndef TEMPO_VALUE_H
#define TEMPO_VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef enum {
    TP_VALUE_BOOLEAN,
    TP_VALUE_NUMBER,
} TpValueType;

typedef struct {
    TpValueType type;
    union {
        bool as_boolean;
        double as_number;
    };
} TpValue;

void tp_value_print(FILE *stream, TpValue value);

typedef struct {
    TpValue *data;
    size_t size;
    size_t capacity;
} TpValueArray;

void         tp_value_array_free(const TpValueArray *array);
TpValueArray tp_value_array_init(void);
void         tp_value_array_write(TpValueArray *array, TpValue value);

#endif
