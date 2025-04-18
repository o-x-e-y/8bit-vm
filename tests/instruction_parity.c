#include <stdbool.h>

#include "../src/headers/instructions.h"
#include "greatest.h"
#include "tests.h"
#include "util.h"

CPU cpu1;
CPU cpu2;

static void initCpus(Opcode op) {
    initCpu(&cpu1);
    initCpu(&cpu2);

    initTestCpu(&cpu1, op);
    initTestCpu(&cpu2, op);
}

static void freeCpus() {
    freeCpu(&cpu1);
    freeCpu(&cpu2);
}

#define TEST_INSTR(op)                                                        \
    TEST TEST_INSTR_##op(void) {                                              \
        initCpus(OP_##op);                                                    \
                                                                              \
        stepCpu(&cpu1);                                                       \
        op(&cpu2);                                                            \
                                                                              \
        ASSERTm("Instruction parity failed for " #op, cpus_eq(&cpu1, &cpu2)); \
                                                                              \
        freeCpus();                                                           \
        PASS();                                                               \
    }

TEST_INSTR(NOOP);
TEST_INSTR(HALT);
TEST_INSTR(EI);
TEST_INSTR(DI);
TEST_INSTR(ET);
TEST_INSTR(DT);
TEST_INSTR(CLRA);
TEST_INSTR(RESET);
TEST_INSTR(LOAD_I);
TEST_INSTR(LOAD_IM);
TEST_INSTR(LOAD_ML);
TEST_INSTR(LOAD_MHL);
TEST_INSTR(LOAD_R0);
TEST_INSTR(LOAD_R1);
TEST_INSTR(LOAD_L);
TEST_INSTR(LOAD_H);
TEST_INSTR(LOAD_L_I);
TEST_INSTR(STORE_IM);
TEST_INSTR(STORE_ML);
TEST_INSTR(STORE_MHL);
TEST_INSTR(STORE_R0);
TEST_INSTR(STORE_R1);
TEST_INSTR(STORE_L);
TEST_INSTR(STORE_H);
TEST_INSTR(LOAD_HL_I);
TEST_INSTR(XCH_IM);
TEST_INSTR(XCH_ML);
TEST_INSTR(XCH_MHL);
TEST_INSTR(XCH_R0);
TEST_INSTR(XCH_R1);
TEST_INSTR(XCH_L);
TEST_INSTR(XCH_H);
TEST_INSTR(ADD_I);
TEST_INSTR(ADD_ACC);
TEST_INSTR(ADD_ML);
TEST_INSTR(ADD_MHL);
TEST_INSTR(ADD_R0);
TEST_INSTR(ADD_R1);
TEST_INSTR(ADD_L);
TEST_INSTR(ADD_H);
TEST_INSTR(ADC_I);
TEST_INSTR(ADC_ACC);
TEST_INSTR(ADC_ML);
TEST_INSTR(ADC_MHL);
TEST_INSTR(ADC_R0);
TEST_INSTR(ADC_R1);
TEST_INSTR(ADC_L);
TEST_INSTR(ADC_H);
TEST_INSTR(SUB_I);
TEST_INSTR(SUB_ACC);
TEST_INSTR(SUB_ML);
TEST_INSTR(SUB_MHL);
TEST_INSTR(SUB_R0);
TEST_INSTR(SUB_R1);
TEST_INSTR(SUB_L);
TEST_INSTR(SUB_H);
TEST_INSTR(SBC_I);
TEST_INSTR(SBC_ACC);
TEST_INSTR(SBC_ML);
TEST_INSTR(SBC_MHL);
TEST_INSTR(SBC_R0);
TEST_INSTR(SBC_R1);
TEST_INSTR(SBC_L);
TEST_INSTR(SBC_H);
TEST_INSTR(INC_HL);
TEST_INSTR(INC_ACC);
TEST_INSTR(INC_ML);
TEST_INSTR(INC_MHL);
TEST_INSTR(INC_R0);
TEST_INSTR(INC_R1);
TEST_INSTR(INC_L);
TEST_INSTR(INC_H);
TEST_INSTR(DEC_HL);
TEST_INSTR(DEC_ACC);
TEST_INSTR(DEC_ML);
TEST_INSTR(DEC_MHL);
TEST_INSTR(DEC_R0);
TEST_INSTR(DEC_R1);
TEST_INSTR(DEC_L);
TEST_INSTR(DEC_H);
TEST_INSTR(NEG_HL);
TEST_INSTR(NEG_ACC);
TEST_INSTR(NEG_ML);
TEST_INSTR(NEG_MHL);
TEST_INSTR(NEG_R0);
TEST_INSTR(NEG_R1);
TEST_INSTR(NEG_L);
TEST_INSTR(NEG_H);
TEST_INSTR(NOT_HL);
TEST_INSTR(NOT_ACC);
TEST_INSTR(NOT_ML);
TEST_INSTR(NOT_MHL);
TEST_INSTR(NOT_R0);
TEST_INSTR(NOT_R1);
TEST_INSTR(NOT_L);
TEST_INSTR(NOT_H);
TEST_INSTR(AND_I);
TEST_INSTR(AND_ACC);
TEST_INSTR(AND_ML);
TEST_INSTR(AND_MHL);
TEST_INSTR(AND_R0);
TEST_INSTR(AND_R1);
TEST_INSTR(AND_L);
TEST_INSTR(AND_H);
TEST_INSTR(OR_I);
TEST_INSTR(OR_ACC);
TEST_INSTR(OR_ML);
TEST_INSTR(OR_MHL);
TEST_INSTR(OR_R0);
TEST_INSTR(OR_R1);
TEST_INSTR(OR_L);
TEST_INSTR(OR_H);
TEST_INSTR(XOR_I);
TEST_INSTR(XOR_ACC);
TEST_INSTR(XOR_ML);
TEST_INSTR(XOR_MHL);
TEST_INSTR(XOR_R0);
TEST_INSTR(XOR_R1);
TEST_INSTR(XOR_L);
TEST_INSTR(XOR_H);
TEST_INSTR(SHL_I);
TEST_INSTR(SHL_ML);
TEST_INSTR(SHL_MHL);
TEST_INSTR(SHL_R0);
TEST_INSTR(SHL_R1);
TEST_INSTR(SHL_L);
TEST_INSTR(SHL_H);
TEST_INSTR(SHR_I);
TEST_INSTR(SHR_ML);
TEST_INSTR(SHR_MHL);
TEST_INSTR(SHR_R0);
TEST_INSTR(SHR_R1);
TEST_INSTR(SHR_L);
TEST_INSTR(SHR_H);
TEST_INSTR(ROL_I);
TEST_INSTR(ROL_ML);
TEST_INSTR(ROL_MHL);
TEST_INSTR(ROL_R0);
TEST_INSTR(ROL_R1);
TEST_INSTR(ROL_L);
TEST_INSTR(ROL_H);
TEST_INSTR(ROR_I);
TEST_INSTR(ROR_ML);
TEST_INSTR(ROR_MHL);
TEST_INSTR(ROR_R0);
TEST_INSTR(ROR_R1);
TEST_INSTR(ROR_L);
TEST_INSTR(ROR_H);
TEST_INSTR(ADDW_I);
TEST_INSTR(ADDW_ACC);
TEST_INSTR(ADDW_R0);
TEST_INSTR(ADDW_R1);
TEST_INSTR(SUBW_I);
TEST_INSTR(SUBW_ACC);
TEST_INSTR(SUBW_R0);
TEST_INSTR(SUBW_R1);
TEST_INSTR(MULW_I);
TEST_INSTR(MULW_ACC);
TEST_INSTR(MULW_R0);
TEST_INSTR(MULW_R1);
TEST_INSTR(DIVW_I);
TEST_INSTR(DIVW_ACC);
TEST_INSTR(DIVW_R0);
TEST_INSTR(DIVW_R1);
TEST_INSTR(JMP);
TEST_INSTR(JIHL);
TEST_INSTR(JHL);
TEST_INSTR(JZ);
TEST_INSTR(JNZ);
TEST_INSTR(JC);
TEST_INSTR(JNC);
TEST_INSTR(JEXT);
TEST_INSTR(CMP_I);
TEST_INSTR(CMP_ACC);
TEST_INSTR(CMP_ML);
TEST_INSTR(CMP_MHL);
TEST_INSTR(CMP_R0);
TEST_INSTR(CMP_R1);
TEST_INSTR(CMP_L);
TEST_INSTR(CMP_H);
TEST_INSTR(PUSH_I);
TEST_INSTR(PUSH_ACC);
TEST_INSTR(PUSH_R0);
TEST_INSTR(PUSH_R1);
TEST_INSTR(PUSH_L);
TEST_INSTR(PUSH_H);
TEST_INSTR(PUSH_BP);
TEST_INSTR(PUSH_FLAGS);
TEST_INSTR(POP_IM);
TEST_INSTR(POP_ACC);
TEST_INSTR(POP_R0);
TEST_INSTR(POP_R1);
TEST_INSTR(POP_L);
TEST_INSTR(POP_H);
TEST_INSTR(POP_BP);
TEST_INSTR(POP_FLAGS);
TEST_INSTR(CALL);
TEST_INSTR(RET);
TEST_INSTR(ENTER);
TEST_INSTR(LEAVE);
TEST_INSTR(LOAD_BPI);
TEST_INSTR(STORE_BPI);
TEST_INSTR(ADD_L_I);
TEST_INSTR(ADD_HL_I);
TEST_INSTR(MIN_I);
TEST_INSTR(MIN_ML);
TEST_INSTR(MIN_MHL);
TEST_INSTR(MIN_R0);
TEST_INSTR(MIN_R1);
TEST_INSTR(MIN_L);
TEST_INSTR(MIN_H);
TEST_INSTR(MAX_I);
TEST_INSTR(MAX_ML);
TEST_INSTR(MAX_MHL);
TEST_INSTR(MAX_R0);
TEST_INSTR(MAX_R1);
TEST_INSTR(MAX_L);
TEST_INSTR(MAX_H);

SUITE(INSTRUCTION_PARITY_SUITE) {
    RUN_TEST(TEST_INSTR_NOOP);
    RUN_TEST(TEST_INSTR_HALT);
    RUN_TEST(TEST_INSTR_EI);
    RUN_TEST(TEST_INSTR_DI);
    RUN_TEST(TEST_INSTR_ET);
    RUN_TEST(TEST_INSTR_DT);
    RUN_TEST(TEST_INSTR_CLRA);
    RUN_TEST(TEST_INSTR_RESET);
    RUN_TEST(TEST_INSTR_LOAD_I);
    RUN_TEST(TEST_INSTR_LOAD_IM);
    RUN_TEST(TEST_INSTR_LOAD_ML);
    RUN_TEST(TEST_INSTR_LOAD_MHL);
    RUN_TEST(TEST_INSTR_LOAD_R0);
    RUN_TEST(TEST_INSTR_LOAD_R1);
    RUN_TEST(TEST_INSTR_LOAD_L);
    RUN_TEST(TEST_INSTR_LOAD_H);
    RUN_TEST(TEST_INSTR_LOAD_L_I);
    RUN_TEST(TEST_INSTR_STORE_IM);
    RUN_TEST(TEST_INSTR_STORE_ML);
    RUN_TEST(TEST_INSTR_STORE_MHL);
    RUN_TEST(TEST_INSTR_STORE_R0);
    RUN_TEST(TEST_INSTR_STORE_R1);
    RUN_TEST(TEST_INSTR_STORE_L);
    RUN_TEST(TEST_INSTR_STORE_H);
    RUN_TEST(TEST_INSTR_LOAD_HL_I);
    RUN_TEST(TEST_INSTR_XCH_IM);
    RUN_TEST(TEST_INSTR_XCH_ML);
    RUN_TEST(TEST_INSTR_XCH_MHL);
    RUN_TEST(TEST_INSTR_XCH_R0);
    RUN_TEST(TEST_INSTR_XCH_R1);
    RUN_TEST(TEST_INSTR_XCH_L);
    RUN_TEST(TEST_INSTR_XCH_H);
    RUN_TEST(TEST_INSTR_ADD_I);
    RUN_TEST(TEST_INSTR_ADD_ACC);
    RUN_TEST(TEST_INSTR_ADD_ML);
    RUN_TEST(TEST_INSTR_ADD_MHL);
    RUN_TEST(TEST_INSTR_ADD_R0);
    RUN_TEST(TEST_INSTR_ADD_R1);
    RUN_TEST(TEST_INSTR_ADD_L);
    RUN_TEST(TEST_INSTR_ADD_H);
    RUN_TEST(TEST_INSTR_ADC_I);
    RUN_TEST(TEST_INSTR_ADC_ACC);
    RUN_TEST(TEST_INSTR_ADC_ML);
    RUN_TEST(TEST_INSTR_ADC_MHL);
    RUN_TEST(TEST_INSTR_ADC_R0);
    RUN_TEST(TEST_INSTR_ADC_R1);
    RUN_TEST(TEST_INSTR_ADC_L);
    RUN_TEST(TEST_INSTR_ADC_H);
    RUN_TEST(TEST_INSTR_SUB_I);
    RUN_TEST(TEST_INSTR_SUB_ACC);
    RUN_TEST(TEST_INSTR_SUB_ML);
    RUN_TEST(TEST_INSTR_SUB_MHL);
    RUN_TEST(TEST_INSTR_SUB_R0);
    RUN_TEST(TEST_INSTR_SUB_R1);
    RUN_TEST(TEST_INSTR_SUB_L);
    RUN_TEST(TEST_INSTR_SUB_H);
    RUN_TEST(TEST_INSTR_SBC_I);
    RUN_TEST(TEST_INSTR_SBC_ACC);
    RUN_TEST(TEST_INSTR_SBC_ML);
    RUN_TEST(TEST_INSTR_SBC_MHL);
    RUN_TEST(TEST_INSTR_SBC_R0);
    RUN_TEST(TEST_INSTR_SBC_R1);
    RUN_TEST(TEST_INSTR_SBC_L);
    RUN_TEST(TEST_INSTR_SBC_H);
    RUN_TEST(TEST_INSTR_INC_HL);
    RUN_TEST(TEST_INSTR_INC_ACC);
    RUN_TEST(TEST_INSTR_INC_ML);
    RUN_TEST(TEST_INSTR_INC_MHL);
    RUN_TEST(TEST_INSTR_INC_R0);
    RUN_TEST(TEST_INSTR_INC_R1);
    RUN_TEST(TEST_INSTR_INC_L);
    RUN_TEST(TEST_INSTR_INC_H);
    RUN_TEST(TEST_INSTR_DEC_HL);
    RUN_TEST(TEST_INSTR_DEC_ACC);
    RUN_TEST(TEST_INSTR_DEC_ML);
    RUN_TEST(TEST_INSTR_DEC_MHL);
    RUN_TEST(TEST_INSTR_DEC_R0);
    RUN_TEST(TEST_INSTR_DEC_R1);
    RUN_TEST(TEST_INSTR_DEC_L);
    RUN_TEST(TEST_INSTR_DEC_H);
    RUN_TEST(TEST_INSTR_NEG_HL);
    RUN_TEST(TEST_INSTR_NEG_ACC);
    RUN_TEST(TEST_INSTR_NEG_ML);
    RUN_TEST(TEST_INSTR_NEG_MHL);
    RUN_TEST(TEST_INSTR_NEG_R0);
    RUN_TEST(TEST_INSTR_NEG_R1);
    RUN_TEST(TEST_INSTR_NEG_L);
    RUN_TEST(TEST_INSTR_NEG_H);
    RUN_TEST(TEST_INSTR_NOT_HL);
    RUN_TEST(TEST_INSTR_NOT_ACC);
    RUN_TEST(TEST_INSTR_NOT_ML);
    RUN_TEST(TEST_INSTR_NOT_MHL);
    RUN_TEST(TEST_INSTR_NOT_R0);
    RUN_TEST(TEST_INSTR_NOT_R1);
    RUN_TEST(TEST_INSTR_NOT_L);
    RUN_TEST(TEST_INSTR_NOT_H);
    RUN_TEST(TEST_INSTR_AND_I);
    RUN_TEST(TEST_INSTR_AND_ACC);
    RUN_TEST(TEST_INSTR_AND_ML);
    RUN_TEST(TEST_INSTR_AND_MHL);
    RUN_TEST(TEST_INSTR_AND_R0);
    RUN_TEST(TEST_INSTR_AND_R1);
    RUN_TEST(TEST_INSTR_AND_L);
    RUN_TEST(TEST_INSTR_AND_H);
    RUN_TEST(TEST_INSTR_OR_I);
    RUN_TEST(TEST_INSTR_OR_ACC);
    RUN_TEST(TEST_INSTR_OR_ML);
    RUN_TEST(TEST_INSTR_OR_MHL);
    RUN_TEST(TEST_INSTR_OR_R0);
    RUN_TEST(TEST_INSTR_OR_R1);
    RUN_TEST(TEST_INSTR_OR_L);
    RUN_TEST(TEST_INSTR_OR_H);
    RUN_TEST(TEST_INSTR_XOR_I);
    RUN_TEST(TEST_INSTR_XOR_ACC);
    RUN_TEST(TEST_INSTR_XOR_ML);
    RUN_TEST(TEST_INSTR_XOR_MHL);
    RUN_TEST(TEST_INSTR_XOR_R0);
    RUN_TEST(TEST_INSTR_XOR_R1);
    RUN_TEST(TEST_INSTR_XOR_L);
    RUN_TEST(TEST_INSTR_XOR_H);
    RUN_TEST(TEST_INSTR_SHL_I);
    RUN_TEST(TEST_INSTR_SHL_ML);
    RUN_TEST(TEST_INSTR_SHL_MHL);
    RUN_TEST(TEST_INSTR_SHL_R0);
    RUN_TEST(TEST_INSTR_SHL_R1);
    RUN_TEST(TEST_INSTR_SHL_L);
    RUN_TEST(TEST_INSTR_SHL_H);
    RUN_TEST(TEST_INSTR_SHR_I);
    RUN_TEST(TEST_INSTR_SHR_ML);
    RUN_TEST(TEST_INSTR_SHR_MHL);
    RUN_TEST(TEST_INSTR_SHR_R0);
    RUN_TEST(TEST_INSTR_SHR_R1);
    RUN_TEST(TEST_INSTR_SHR_L);
    RUN_TEST(TEST_INSTR_SHR_H);
    RUN_TEST(TEST_INSTR_ROL_I);
    RUN_TEST(TEST_INSTR_ROL_ML);
    RUN_TEST(TEST_INSTR_ROL_MHL);
    RUN_TEST(TEST_INSTR_ROL_R0);
    RUN_TEST(TEST_INSTR_ROL_R1);
    RUN_TEST(TEST_INSTR_ROL_L);
    RUN_TEST(TEST_INSTR_ROL_H);
    RUN_TEST(TEST_INSTR_ROR_I);
    RUN_TEST(TEST_INSTR_ROR_ML);
    RUN_TEST(TEST_INSTR_ROR_MHL);
    RUN_TEST(TEST_INSTR_ROR_R0);
    RUN_TEST(TEST_INSTR_ROR_R1);
    RUN_TEST(TEST_INSTR_ROR_L);
    RUN_TEST(TEST_INSTR_ROR_H);
    RUN_TEST(TEST_INSTR_ADDW_I);
    RUN_TEST(TEST_INSTR_ADDW_ACC);
    RUN_TEST(TEST_INSTR_ADDW_R0);
    RUN_TEST(TEST_INSTR_ADDW_R1);
    RUN_TEST(TEST_INSTR_SUBW_I);
    RUN_TEST(TEST_INSTR_SUBW_ACC);
    RUN_TEST(TEST_INSTR_SUBW_R0);
    RUN_TEST(TEST_INSTR_SUBW_R1);
    RUN_TEST(TEST_INSTR_MULW_I);
    RUN_TEST(TEST_INSTR_MULW_ACC);
    RUN_TEST(TEST_INSTR_MULW_R0);
    RUN_TEST(TEST_INSTR_MULW_R1);
    RUN_TEST(TEST_INSTR_DIVW_I);
    RUN_TEST(TEST_INSTR_DIVW_ACC);
    RUN_TEST(TEST_INSTR_DIVW_R0);
    RUN_TEST(TEST_INSTR_DIVW_R1);
    RUN_TEST(TEST_INSTR_JMP);
    RUN_TEST(TEST_INSTR_JIHL);
    RUN_TEST(TEST_INSTR_JHL);
    RUN_TEST(TEST_INSTR_JZ);
    RUN_TEST(TEST_INSTR_JNZ);
    RUN_TEST(TEST_INSTR_JC);
    RUN_TEST(TEST_INSTR_JNC);
    RUN_TEST(TEST_INSTR_JEXT);
    RUN_TEST(TEST_INSTR_CMP_I);
    RUN_TEST(TEST_INSTR_CMP_ACC);
    RUN_TEST(TEST_INSTR_CMP_ML);
    RUN_TEST(TEST_INSTR_CMP_MHL);
    RUN_TEST(TEST_INSTR_CMP_R0);
    RUN_TEST(TEST_INSTR_CMP_R1);
    RUN_TEST(TEST_INSTR_CMP_L);
    RUN_TEST(TEST_INSTR_CMP_H);
    RUN_TEST(TEST_INSTR_PUSH_I);
    RUN_TEST(TEST_INSTR_PUSH_ACC);
    RUN_TEST(TEST_INSTR_PUSH_R0);
    RUN_TEST(TEST_INSTR_PUSH_R1);
    RUN_TEST(TEST_INSTR_PUSH_L);
    RUN_TEST(TEST_INSTR_PUSH_H);
    RUN_TEST(TEST_INSTR_PUSH_BP);
    RUN_TEST(TEST_INSTR_PUSH_FLAGS);
    RUN_TEST(TEST_INSTR_POP_IM);
    RUN_TEST(TEST_INSTR_POP_ACC);
    RUN_TEST(TEST_INSTR_POP_R0);
    RUN_TEST(TEST_INSTR_POP_R1);
    RUN_TEST(TEST_INSTR_POP_L);
    RUN_TEST(TEST_INSTR_POP_H);
    RUN_TEST(TEST_INSTR_POP_BP);
    RUN_TEST(TEST_INSTR_POP_FLAGS);
    RUN_TEST(TEST_INSTR_CALL);
    RUN_TEST(TEST_INSTR_RET);
    RUN_TEST(TEST_INSTR_ENTER);
    RUN_TEST(TEST_INSTR_LEAVE);
    RUN_TEST(TEST_INSTR_LOAD_BPI);
    RUN_TEST(TEST_INSTR_STORE_BPI);
    RUN_TEST(TEST_INSTR_ADD_L_I);
    RUN_TEST(TEST_INSTR_ADD_HL_I);
    RUN_TEST(TEST_INSTR_MIN_I);
    RUN_TEST(TEST_INSTR_MIN_ML);
    RUN_TEST(TEST_INSTR_MIN_MHL);
    RUN_TEST(TEST_INSTR_MIN_R0);
    RUN_TEST(TEST_INSTR_MIN_R1);
    RUN_TEST(TEST_INSTR_MIN_L);
    RUN_TEST(TEST_INSTR_MIN_H);
    RUN_TEST(TEST_INSTR_MAX_I);
    RUN_TEST(TEST_INSTR_MAX_ML);
    RUN_TEST(TEST_INSTR_MAX_MHL);
    RUN_TEST(TEST_INSTR_MAX_R0);
    RUN_TEST(TEST_INSTR_MAX_R1);
    RUN_TEST(TEST_INSTR_MAX_L);
    RUN_TEST(TEST_INSTR_MAX_H);
}