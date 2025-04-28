#include "headers/cpu.h"

#include <stdio.h>
#include <stdlib.h>

#include "headers/debug.h"
#include "headers/instructions.h"

void initCpu(CPU* cpu) {
    PC = PROGRAM_START;
    FLAGS = 0;
    ACC = 0;
    R0 = 0;
    R1 = 0;
    H = 0;
    L = 0;
    SP = 0;
    BP = 0;

    for (size_t i = 0; i < STACK_SIZE; ++i) {
        STACK(i) = 0;
    }

    uint8_t* memory = (uint8_t*)calloc(MEMORY_SIZE, sizeof(uint8_t));

    if (memory == NULL) {
        exit(1);
    }

    cpu->memory = memory;
}

void freeCpu(CPU* cpu) {
    if (cpu != NULL && cpu->memory != NULL) {
        free(cpu->memory);
    }
}

void resetCpu(CPU* cpu) {
    freeCpu(cpu);
    initCpu(cpu);
}

void loadProgram(CPU* cpu, const uint8_t* program, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        MEMORY(i) = program[i];
    }
}

int runCpu(CPU* cpu) {
    while (stepCpu(cpu) != OP_HALT) {
        if (get_tf(FLAGS)) {
            printCpu(cpu);
            printStack(cpu, SP + 5);
            printf("Next opcode: ");
            printNextOperation(&MEMORY(PC));
            printf("\n");
            getchar();
        }
    }

    return 0;
}

int stepCpu(CPU* cpu) {
    OP_TABLE[MEMORY(PC)](cpu);

    return MEMORY(PC);
}
