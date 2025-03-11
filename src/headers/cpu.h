#ifndef __OXEY_CCE_CPU_H
#define __OXEY_CCE_CPU_H

#include "common.h"
#include "flags.h"

#define STACK_SIZE 256
#define MEMORY_SIZE 256 * 256
#define SIGN_BIT 0b10000000

#define PC cpu->program_counter
#define ACC cpu->accumulator
#define R0 cpu->registers.reg_0
#define R1 cpu->registers.reg_1
#define H cpu->registers.reg_H
#define L cpu->registers.reg_L
#define HL (((uint16_t)H << 8) | (uint16_t)(L))
#define FLAGS cpu->flags
#define MODIFY_FLAG(func) FLAGS = func(FLAGS)
#define UPDATE_FLAGS(cmp, flag)        \
    do {                               \
        if (cmp)                       \
            MODIFY_FLAG(set_##flag);   \
        else                           \
            MODIFY_FLAG(unset_##flag); \
    } while (0)

#define SP cpu->stackptr
#define BP cpu->baseptr
#define STACK(idx) cpu->stack[idx]
#define MEMORY(idx) cpu->memory[idx]

#define CARRY_FLAG() get_cf(FLAGS)

typedef struct Registers {
    uint8_t reg_0;
    uint8_t reg_1;
    uint8_t reg_H;
    uint8_t reg_L;
} Registers;

typedef struct CPU {
    uint16_t program_counter;
    uint8_t accumulator;
    Registers registers;
    Flags flags;
    uint8_t stack[STACK_SIZE];
    uint8_t stackptr;
    uint8_t baseptr;
    uint8_t* memory;
} CPU;

void initCpu(CPU* cpu);
void freeCpu(CPU* cpu);
void resetCpu(CPU* cpu);
void loadProgram(CPU* cpu, const uint8_t* program, uint16_t length);

int runCpu(CPU* cpu);
int stepCpu(CPU* cpu);

#endif
