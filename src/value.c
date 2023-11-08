#include "tempo/value.h"

#include <stdio.h>
#include <stdlib.h>

void tp_value_print(FILE *stream, TpValue value) {
    switch (value.type) {
    case TP_VALUE_BOOLEAN:
        fprintf(stream, "%s", value.as_boolean ? "true" : "false");
        break;
    case TP_VALUE_NUMBER:
        fprintf(stream, "%g", value.as_number);
        break;
    }
}

void tp_value_array_free(const TpValueArray *array) {
    free(array->data);
}

TpValueArray tp_value_array_init(void) {
    return (TpValueArray) {
        .data = NULL,
        .size = 0,
        .capacity = 0,
    };
}

void tp_value_array_write(TpValueArray *array, TpValue value) {
    if (array->size + 1 > array->capacity) {
        array->capacity = array->capacity < 8 ? 8 : array->capacity * 2;
        array->data = realloc(array->data, array->capacity * sizeof(TpValue));
        if (array->data == NULL) {
            fprintf(stderr, "[ERROR] Failed to reallocate memory\n");
            exit(1);
        }
    }
    array->data[array->size] = value;
    array->size += 1;
}
