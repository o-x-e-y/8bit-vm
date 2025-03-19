#ifndef __OXEY_CCE_ASSEMBLER_H
#define __OXEY_CCE_ASSEMBLER_H

#include "common.h"
#include "oslice.h"

typedef struct Executable {
    uint8_t* executable;
    size_t size;
} Executable;

void initProgram(Executable* exec);
void freeProgram(Executable* exec);
Executable assemble(slice_t program);

#endif
