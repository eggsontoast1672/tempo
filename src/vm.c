#include "tempo/vm.h"

#include "tempo/chunk.h"
#include "tempo/parser.h"
#include "tempo/value.h"

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
        switch (*vm.ip++) {
        case TP_BYTE_ADD: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER || b.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operands to '+' must be numbers\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_number = a.as_number + b.as_number,
            });
        }
        break;
        case TP_BYTE_CONSTANT:
            tp_vm_stack_push(&vm, chunk->constants.data[*vm.ip++]);
            break;
        case TP_BYTE_DIVIDE: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER || b.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operands to '/' must be numbers\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_number = a.as_number / b.as_number,
            });
        }
        break;
        case TP_BYTE_EQUAL: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            switch (a.type) {
            case TP_VALUE_BOOLEAN:
                if (b.type != TP_VALUE_BOOLEAN) {
                    fprintf(stderr, "Cannot compare boolean to non-boolean\n");
                    return TP_INTERPRET_RUNTIME_ERROR;
                }
                tp_vm_stack_push(&vm, (TpValue) {
                    .type = TP_VALUE_BOOLEAN,
                    .as_boolean = a.as_boolean == b.as_boolean,
                });
                break;
            case TP_VALUE_NUMBER:
                if (b.type != TP_VALUE_NUMBER) {
                    fprintf(stderr, "Cannot compare number to non-number\n");
                    return TP_INTERPRET_RUNTIME_ERROR;
                }
                tp_vm_stack_push(&vm, (TpValue) {
                    .type = TP_VALUE_BOOLEAN,
                    .as_boolean = a.as_number == b.as_number,
                });
                break;
            }
        }
        break;
        case TP_BYTE_FALSE:
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_BOOLEAN,
                .as_boolean = false,
            });
            break;
        case TP_BYTE_GREATER: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER || b.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operands to '>' must be numbers\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_boolean = a.as_number > a.as_number,
            });
        }
        break;
        case TP_BYTE_LESS: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER || b.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operands to '<' must be numbers\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_boolean = a.as_number < a.as_number,
            });
        }
        break;
        case TP_BYTE_MULTIPLY: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER || b.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operands to '*' must be numbers\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_number = a.as_number * b.as_number,
            });
        }
        break;
        case TP_BYTE_NEGATE: {
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operand to '-' must be a number\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_number = -a.as_number,
            });
        }
        break;
        case TP_BYTE_NOT: {
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_BOOLEAN) {
                fprintf(stderr, "Operand to '!' must be a boolean\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_BOOLEAN,
                .as_boolean = !a.as_boolean,
            });
        }
        break;
        case TP_BYTE_PRINT: {
            TpValue a = tp_vm_stack_pop(&vm);
            tp_value_print(stdout, a);
        }
        break;
        case TP_BYTE_RETURN:
            break;
        case TP_BYTE_SUBTRACT: {
            TpValue b = tp_vm_stack_pop(&vm);
            TpValue a = tp_vm_stack_pop(&vm);
            if (a.type != TP_VALUE_NUMBER || b.type != TP_VALUE_NUMBER) {
                fprintf(stderr, "Operands to '-' must be numbers\n");
                return TP_INTERPRET_RUNTIME_ERROR;
            }
            tp_vm_stack_push(&vm, (TpValue) {
                .type = TP_VALUE_NUMBER,
                .as_number = a.as_number - b.as_number,
            });
        }
        break;
        }
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
