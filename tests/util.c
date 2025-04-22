#include "util.h"

#include <string.h>

void initTestCpu(CPU* cpu, Opcode op) {
    cpu->accumulator = 3;
    cpu->registers.reg_0 = 4;
    cpu->registers.reg_1 = 5;
    cpu->registers.reg_H = 6;
    cpu->registers.reg_L = 7;
    cpu->program_counter = 8;
    cpu->flags = 123;
    cpu->baseptr = 2;

    cpu->memory[cpu->program_counter] = op;

    for (int i = 1; i < 4; ++i) {
        cpu->memory[cpu->program_counter + i] = i;
        cpu->stack[cpu->stackptr++] = i;
    }
}

bool cpus_eq(CPU* cpu1, CPU* cpu2) {
    if (cpu1->accumulator != cpu2->accumulator || cpu1->registers.reg_0 != cpu2->registers.reg_0 ||
        cpu1->registers.reg_1 != cpu2->registers.reg_1 ||
        cpu1->registers.reg_H != cpu2->registers.reg_H ||
        cpu1->registers.reg_L != cpu2->registers.reg_L ||
        cpu1->program_counter != cpu2->program_counter || cpu1->flags != cpu2->flags ||
        cpu1->stackptr != cpu2->stackptr || cpu1->baseptr != cpu2->baseptr ||
        memcmp(cpu1->stack, cpu2->stack, STACK_SIZE) ||
        memcmp(cpu1->memory, cpu2->memory, MEMORY_SIZE)) {
        return false;
    }
    return true;
}