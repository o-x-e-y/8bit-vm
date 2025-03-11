#include "headers/cpu.h"
#include "headers/instructions.h"
#include "headers/debug.h"

int main(int argc, char** argv) {
    CPU cpu;
    initCpu(&cpu);

    // uint8_t program[] = {
    //     OP_LOAD_I, 5,
    //     OP_ADD_I, 3,
    //     OP_STORE_IM, 0x12, 0x34,
    //     OP_LOAD_I, 0,
    //     OP_LOAD_I, 0x12,
    //     OP_STORE_H,
    //     OP_LOAD_I, 0x34,
    //     OP_STORE_L,
    //     OP_LOAD_MHL,
    //     OP_HALT
    // };
    
    uint8_t __fibonacci[] = {
        // init registers with the first two fibonacci values
        OP_LOAD_I, 0,
        OP_STORE_L,
        OP_LOAD_I, 1,
        OP_STORE_H,
        
        // init counter register, we're finding fibbonacci number 5
        OP_LOAD_I, 10,
        OP_STORE_R0,
        
        // Reset accumulator and add both old values to it
        OP_LOAD_I, 0,
        OP_ADD_L,
        OP_ADD_H,
        
        // new is now in accumulator, we need to move H -> L and acc -> H
        OP_XCH_H,
        OP_STORE_L,
        
        // Decrement counter, jump to instruction 9 (start of loop) if not zero
        OP_DEC_R0,
        OP_JNZ, 0, 9,
        
        // Load final value into the accumulator
        OP_LOAD_H,
        
        OP_HALT
    };
    
    uint8_t test[] = {
        OP_ET,
        OP_PUSH_I, 5,
        OP_PUSH_I, 3,
        OP_CALL, 0, 9,
        OP_HALT,
        OP_ENTER, 4,
        OP_LEAVE,
        OP_RET,
    };

    loadProgram(&cpu, test, sizeof(__fibonacci));
    
    runCpu(&cpu);
    
    printCpu(&cpu);
    printStack(&cpu, 10);

    freeCpu(&cpu);
    return 0;
}
