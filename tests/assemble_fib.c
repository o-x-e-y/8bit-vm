#include "../src/headers/assembler.h"
#include "../src/headers/instructions.h"
#include "../src/headers/util.h"
#include "greatest.h"
#include "util.h"

// clang-format off

const uint8_t fibonacci[] = {
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
    OP_JNZ, 1, 9,

    // Load final value into the accumulator
    OP_LOAD_H,

    OP_HALT,
    OP_HALT
};

// clang-format on

TEST assemble_fibonacci(void) {
    CPU cpu1;
    CPU cpu2;

    initCpu(&cpu1);
    initCpu(&cpu2);
    
    const char* path = "./programs/fibonacci.casm";

    string_t program = read_file_to_str(path);

    const Executable exec = assemble(from_str_slice(program), from_cstr_slice(path, strlen(path)));

    // load compiled assembly into cpu1
    loadProgram(&cpu1, exec.executable, exec.size);

    const size_t size = PROGRAM_START + sizeof(fibonacci) + 1;
    const Executable exec2 =
        (Executable){.executable = calloc(size, sizeof(uint8_t)), .size = size};

    memcpy((exec2.executable + PROGRAM_START), fibonacci, sizeof(fibonacci));

    loadProgram(&cpu2, exec2.executable, exec2.size);

    ASSERT(cpus_eq(&cpu1, &cpu2));

    while (stepCpu(&cpu1) != OP_HALT && stepCpu(&cpu2) != OP_HALT) {
        ASSERT(cpus_eq(&cpu1, &cpu2));
    }

    PASS();
}

SUITE(ASSEMBLE_FIB_SUITE) { RUN_TEST(assemble_fibonacci); }
