#include "headers/assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/cpu.h"
#include "headers/tokenizer.h"

void initProgram(Executable* exec) {
    uint8_t* memory = (uint8_t*)calloc(MEMORY_SIZE, sizeof(uint8_t));

    if (memory == NULL) {
        exit(1);
    }

    exec->executable = memory;
}

void freeProgram(Executable* exec) {
    if (exec->executable != NULL) {
        free(exec->executable);
    }
}

Executable assemble(slice_t program) {}
