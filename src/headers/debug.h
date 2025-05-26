#ifndef __OXEY_CCE_DEBUG_H
#define __OXEY_CCE_DEBUG_H

#include "cpu.h"
#include "instructions.h"
#include "tokenizer.h"

void printOpcode(Opcode op);
void printNextOperation(const uint8_t* memory);
void printCpu(CPU* cpu);
void printFlags(CPU* cpu);
void printStack(CPU* cpu, uint8_t size);
void printToken(void* token);
void printTokenLines(TokenLines* tokenLines);

#endif
