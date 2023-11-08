#include "tempo/vm.h"

#include "tempo/chunk.h"
#include "tempo/compiler.h"

TpVm tp_vm_init(const TpChunk *chunk) {
    TpVm vm = {
        .chunk = chunk,
        .ip = chunk->code,
    };
    vm.sp = vm.stack;
    return vm;
}

TpInterpretResult tp_interpret_chunk(TpChunk *chunk) {
    TpVm vm = tp_vm_init(chunk);
    while (*vm.ip != TP_BYTE_RETURN) {
    }
    return TP_INTERPRET_OK;
}

TpInterpretResult tp_interpret_source(const char *source) {
    TpChunk chunk = tp_chunk_init();
    if (!tp_compile_source(source, &chunk)) {
        return TP_INTERPRET_COMPILE_ERROR;
    }
    TpInterpretResult result = tp_interpret_chunk(&chunk);
    tp_chunk_free(&chunk);
    return result;
}
