// #include "headers/assembler.h"

#include <stdio.h>
#include <stdlib.h>

#include "headers/assembler.h"
#include "headers/cpu.h"
#include "headers/debug.h"
#include "headers/util.h"

#define USAGE "USAGE: build/vm <filename>.casm\n"

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf(USAGE);
    } else if (argc == 2) {
        CPU cpu;
        initCpu(&cpu);

        const char* filename = argv[1];
        const string_t programStr = read_file_to_str(filename);
        const Executable exec =
            assemble(from_str_slice(programStr), from_cstr_slice(filename, strlen(filename)));

        printf("created executable with size %lu\n", exec.size - PROGRAM_START);

        loadProgram(&cpu, exec.executable, exec.size);

        runCpu(&cpu);

        printCpu(&cpu);
        printStack(&cpu, 10);

        free(exec.executable);
        free_str((string_t*)&programStr);
        freeCpu(&cpu);
    } else {
        printf("compiling multiple assembly files is currently unsupported\n");
        printf(USAGE);
    }

    return 0;
}
