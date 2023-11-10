#ifndef TEMPO_VM_H
#define TEMPO_VM_H

#include "tempo/chunk.h"

typedef enum {
    TP_INTERPRET_OK,
    TP_INTERPRET_COMPILE_ERROR,
    TP_INTERPRET_RUNTIME_ERROR,
} TpInterpretResult;

#define TP_VM_STACK_SIZE 1024

typedef struct {
    const TpChunk *chunk;
    uint8_t       *ip;
    TpValue       stack[TP_VM_STACK_SIZE];
    TpValue       *sp;
} TpVm;

TpVm    tp_vm_init(const TpChunk *chunk);
TpValue tp_vm_stack_pop(TpVm *vm);
TpValue tp_vm_stack_push(TpVm *vm, TpValue value);

TpInterpretResult tp_interpret_chunk(TpChunk *chunk);
TpInterpretResult tp_interpret_source(const char *source);

#endif
