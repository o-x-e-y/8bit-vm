// #include "headers/assembler.h"

#include <stdio.h>
#include <stdlib.h>

#include "headers/assembler.h"
#include "headers/cpu.h"
#include "headers/debug.h"
#include "headers/instructions.h"
#include "headers/tokenizer.h"
#include "headers/util.h"

int main(int argc, char** argv) {
    CPU cpu;
    initCpu(&cpu);

    // clang-format off

    uint8_t fibonacci[] = {
        OP_ET,
        // init registers with the first two fibonacci values
        OP_CLRA, // OP_LOAD_I, 0,
        OP_STORE_L,
        OP_LOAD_I, 1,
        OP_STORE_H,

        // init counter register, we're finding fibbonacci number 10
        OP_LOAD_I, 10,
        OP_STORE_R0,

        // Reset accumulator and add both old values to it
        OP_CLRA, // OP_LOAD_I, 0,
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

    // clang-format on

    // loadProgram(&cpu, fibonacci, sizeof(fibonacci));

    // runCpu(&cpu);

    // printCpu(&cpu);
    // printStack(&cpu, 10);

    const string_t programStr = read_file_to_str("./programs/collatz.casm");
    const slice_t program = from_str_slice(programStr);

    TokenLines tokenLines = tokenizeProgram(program);
    vec_iter_t lineIter = iter_from_vec(&tokenLines.lines);

    TokenLine* line;
    Token* token;

    while ((line = iter_next(&lineIter))) {
        vec_iter_t tokenIter = iter_from_vec(&line->tokens);
        while ((token = iter_next(&tokenIter))) {
            printf("tok: ");
            printToken(token);
            printf(", \"%.*s\"\n", (int)token->substr.len, token->substr.str);
        }
        printf("\n");
    }

    const Executable exec = assemble(program);

    printf("created executable with size %lu\n", exec.size);

    loadProgram(&cpu, exec.executable, exec.size);

    runCpu(&cpu);

    printCpu(&cpu);
    printStack(&cpu, 10);

    freeTokenLines(&tokenLines);

    free(exec.executable);
    free_str((string_t*)&programStr);
    freeCpu(&cpu);

    return 0;
}
