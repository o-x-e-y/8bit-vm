#include <stdio.h>

#include "headers/cpu.h"
#include "headers/debug.h"
#include "headers/disassembler.h"

void disassemble(Executable exec) {
    uint8_t* e = exec.executable + PROGRAM_START;
    uint8_t* b;
    while ((b = e++) && *b != 0) {
        // this shit doesn't work yet
        printNextOperation(b);
        printf("\n");
    }
}
