#include "../src/headers/instructions.h"
#include "greatest.h"
#include "util.h"

TEST TEST_ADD_IMPL(void) {
    CPU cpu;

    initCpu(&cpu);
    initTestCpu(&cpu, OP_ADD_H);

    uint8_t h = cpu.registers.reg_H;
    uint8_t acc = cpu.accumulator;

    stepCpu(&cpu);

    ASSERT_EQ(cpu.accumulator, h + acc);

    uint8_t imm = 5;
    cpu.memory[cpu.program_counter] = OP_ADD_I;
    cpu.memory[cpu.program_counter + 1] = imm;

    stepCpu(&cpu);

    ASSERT_EQ(cpu.accumulator, h + acc + imm);

    cpu.memory[cpu.program_counter] = OP_ADD_R0;

    uint8_t r0 = cpu.registers.reg_0;

    stepCpu(&cpu);

    ASSERT_EQ(cpu.accumulator, h + acc + imm + r0);

    freeCpu(&cpu);

    PASS();
}

TEST TEST_ADC_IMPL(void) {
    CPU cpu;

    initCpu(&cpu);
    initTestCpu(&cpu, OP_ADC_R0);

    cpu.flags = unset_cf(cpu.flags);

    uint8_t r0 = cpu.registers.reg_0;
    uint8_t acc = cpu.accumulator;

    stepCpu(&cpu);

    ASSERT_EQ(cpu.accumulator, r0 + acc);

    cpu.flags = set_cf(cpu.flags);

    uint8_t imm = 5;
    cpu.memory[cpu.program_counter] = OP_ADC_I;
    cpu.memory[cpu.program_counter + 1] = imm;

    stepCpu(&cpu);

    ASSERT_EQ(cpu.accumulator, r0 + acc + imm + 1);

    freeCpu(&cpu);

    PASS();
}

TEST TEST_PUSH_IMPL(void) {
    CPU cpu;

    initCpu(&cpu);
    initTestCpu(&cpu, OP_PUSH_R0);

    uint8_t sp = cpu.stackptr;

    stepCpu(&cpu);

    ASSERT_EQ(cpu.stackptr, sp + 1);
    ASSERT_EQ(cpu.stack[sp], cpu.registers.reg_0);

    freeCpu(&cpu);

    PASS();
}

SUITE(INSTRUCTION_FUNCTIONALITY_SUITE) {
    RUN_TEST(TEST_ADD_IMPL);
    RUN_TEST(TEST_ADC_IMPL);
    RUN_TEST(TEST_PUSH_IMPL);
}
