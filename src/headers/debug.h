#ifndef __OXEY_CCE_DEBUG_H
#define __OXEY_CCE_DEBUG_H

#include "cpu.h"
#include "instructions.h"

void printOpcode(Opcode op);
void printCpu(CPU* cpu);
void printStack(CPU* cpu, uint8_t size);
void printToken(void* token);

#endif
