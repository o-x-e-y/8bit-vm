#include "headers/debug.h"

#include <stdio.h>

#include "headers/tokenizer.h"

// clang-format off

void printOpcode(Opcode op) {
    switch (op) {
        case OP_NOOP: printf("NOOP"); break;
        case OP_HALT: printf("HALT"); break;
        case OP_EI: printf("EI"); break;
        case OP_DI: printf("DI"); break;
        case OP_ET: printf("ET"); break;
        case OP_DT: printf("DT"); break;
        case OP_CLRA: printf("CLRA"); break;
        case OP_RESET: printf("RESET"); break;
        case OP_LOAD_I: printf("LOAD_I"); break;
        case OP_LOAD_IMHL: printf("LOAD_IMHL"); break;
        case OP_LOAD_ML: printf("LOAD_ML"); break;
        case OP_LOAD_MHL: printf("LOAD_MHL"); break;
        case OP_LOAD_R0: printf("LOAD_R0"); break;
        case OP_LOAD_R1: printf("LOAD_R1"); break;
        case OP_LOAD_L: printf("LOAD_L"); break;
        case OP_LOAD_H: printf("LOAD_H"); break;
        case OP_STORE_IM: printf("STORE_IM"); break;
        case OP_STORE_IMHL: printf("STORE_IMHL"); break;
        case OP_STORE_ML: printf("STORE_ML"); break;
        case OP_STORE_MHL: printf("STORE_MHL"); break;
        case OP_STORE_R0: printf("STORE_R0"); break;
        case OP_STORE_R1: printf("STORE_R1"); break;
        case OP_STORE_L: printf("STORE_L"); break;
        case OP_STORE_H: printf("STORE_H"); break;
        case OP_XCH_ML: printf("XCH_ML"); break;
        case OP_XCH_MHL: printf("XCH_MHL"); break;
        case OP_XCH_R0: printf("XCH_R0"); break;
        case OP_XCH_R1: printf("XCH_R1"); break;
        case OP_XCH_L: printf("XCH_L"); break;
        case OP_XCH_H: printf("XCH_H"); break;
        case OP_ADD_I: printf("ADD_I"); break;
        case OP_ADD_A: printf("ADD_A"); break;
        case OP_ADD_ML: printf("ADD_ML"); break;
        case OP_ADD_MHL: printf("ADD_MHL"); break;
        case OP_ADD_R0: printf("ADD_R0"); break;
        case OP_ADD_R1: printf("ADD_R1"); break;
        case OP_ADD_L: printf("ADD_L"); break;
        case OP_ADD_H: printf("ADD_H"); break;
        case OP_ADC_I: printf("ADC_I"); break;
        case OP_ADC_A: printf("ADC_A"); break;
        case OP_ADC_ML: printf("ADC_ML"); break;
        case OP_ADC_MHL: printf("ADC_MHL"); break;
        case OP_ADC_R0: printf("ADC_R0"); break;
        case OP_ADC_R1: printf("ADC_R1"); break;
        case OP_ADC_L: printf("ADC_L"); break;
        case OP_ADC_H: printf("ADC_H"); break;
        case OP_SUB_I: printf("SUB_I"); break;
        case OP_SUB_A: printf("SUB_A"); break;
        case OP_SUB_ML: printf("SUB_ML"); break;
        case OP_SUB_MHL: printf("SUB_MHL"); break;
        case OP_SUB_R0: printf("SUB_R0"); break;
        case OP_SUB_R1: printf("SUB_R1"); break;
        case OP_SUB_L: printf("SUB_L"); break;
        case OP_SUB_H: printf("SUB_H"); break;
        case OP_SBC_I: printf("SBC_I"); break;
        case OP_SBC_A: printf("SBC_A"); break;
        case OP_SBC_ML: printf("SBC_ML"); break;
        case OP_SBC_MHL: printf("SBC_MHL"); break;
        case OP_SBC_R0: printf("SBC_R0"); break;
        case OP_SBC_R1: printf("SBC_R1"); break;
        case OP_SBC_L: printf("SBC_L"); break;
        case OP_SBC_H: printf("SBC_H"); break;
        case OP_INC_A: printf("INC_A"); break;
        case OP_INC_ML: printf("INC_ML"); break;
        case OP_INC_MHL: printf("INC_MHL"); break;
        case OP_INC_R0: printf("INC_R0"); break;
        case OP_INC_R1: printf("INC_R1"); break;
        case OP_INC_L: printf("INC_L"); break;
        case OP_INC_H: printf("INC_H"); break;
        case OP_DEC_A: printf("DEC_A"); break;
        case OP_DEC_ML: printf("DEC_ML"); break;
        case OP_DEC_MHL: printf("DEC_MHL"); break;
        case OP_DEC_R0: printf("DEC_R0"); break;
        case OP_DEC_R1: printf("DEC_R1"); break;
        case OP_DEC_L: printf("DEC_L"); break;
        case OP_DEC_H: printf("DEC_H"); break;
        case OP_NEG_A: printf("NEG_A"); break;
        case OP_NEG_ML: printf("NEG_ML"); break;
        case OP_NEG_MHL: printf("NEG_MHL"); break;
        case OP_NEG_R0: printf("NEG_R0"); break;
        case OP_NEG_R1: printf("NEG_R1"); break;
        case OP_NEG_L: printf("NEG_L"); break;
        case OP_NEG_H: printf("NEG_H"); break;
        case OP_NOT_A: printf("NOT_A"); break;
        case OP_NOT_ML: printf("NOT_ML"); break;
        case OP_NOT_MHL: printf("NOT_MHL"); break;
        case OP_NOT_R0: printf("NOT_R0"); break;
        case OP_NOT_R1: printf("NOT_R1"); break;
        case OP_NOT_L: printf("NOT_L"); break;
        case OP_NOT_H: printf("NOT_H"); break;
        case OP_AND_I: printf("AND_I"); break;
        case OP_AND_A: printf("AND_A"); break;
        case OP_AND_ML: printf("AND_ML"); break;
        case OP_AND_MHL: printf("AND_MHL"); break;
        case OP_AND_R0: printf("AND_R0"); break;
        case OP_AND_R1: printf("AND_R1"); break;
        case OP_AND_L: printf("AND_L"); break;
        case OP_AND_H: printf("AND_H"); break;
        case OP_OR_I: printf("OR_I"); break;
        case OP_OR_A: printf("OR_A"); break;
        case OP_OR_ML: printf("OR_ML"); break;
        case OP_OR_MHL: printf("OR_MHL"); break;
        case OP_OR_R0: printf("OR_R0"); break;
        case OP_OR_R1: printf("OR_R1"); break;
        case OP_OR_L: printf("OR_L"); break;
        case OP_OR_H: printf("OR_H"); break;
        case OP_XOR_I: printf("XOR_I"); break;
        case OP_XOR_A: printf("XOR_A"); break;
        case OP_XOR_ML: printf("XOR_ML"); break;
        case OP_XOR_MHL: printf("XOR_MHL"); break;
        case OP_XOR_R0: printf("XOR_R0"); break;
        case OP_XOR_R1: printf("XOR_R1"); break;
        case OP_XOR_L: printf("XOR_L"); break;
        case OP_XOR_H: printf("XOR_H"); break;
        case OP_SHL_I: printf("SHL_I"); break;
        case OP_SHL_ML: printf("SHL_ML"); break;
        case OP_SHL_MHL: printf("SHL_MHL"); break;
        case OP_SHL_R0: printf("SHL_R0"); break;
        case OP_SHL_R1: printf("SHL_R1"); break;
        case OP_SHL_L: printf("SHL_L"); break;
        case OP_SHL_H: printf("SHL_H"); break;
        case OP_SHR_I: printf("SHR_I"); break;
        case OP_SHR_ML: printf("SHR_ML"); break;
        case OP_SHR_MHL: printf("SHR_MHL"); break;
        case OP_SHR_R0: printf("SHR_R0"); break;
        case OP_SHR_R1: printf("SHR_R1"); break;
        case OP_SHR_L: printf("SHR_L"); break;
        case OP_SHR_H: printf("SHR_H"); break;
        case OP_ROL_I: printf("ROL_I"); break;
        case OP_ROL_ML: printf("ROL_ML"); break;
        case OP_ROL_MHL: printf("ROL_MHL"); break;
        case OP_ROL_R0: printf("ROL_R0"); break;
        case OP_ROL_R1: printf("ROL_R1"); break;
        case OP_ROL_L: printf("ROL_L"); break;
        case OP_ROL_H: printf("ROL_H"); break;
        case OP_ROR_I: printf("ROR_I"); break;
        case OP_ROR_ML: printf("ROR_ML"); break;
        case OP_ROR_MHL: printf("ROR_MHL"); break;
        case OP_ROR_R0: printf("ROR_R0"); break;
        case OP_ROR_R1: printf("ROR_R1"); break;
        case OP_ROR_L: printf("ROR_L"); break;
        case OP_ROR_H: printf("ROR_H"); break;
        case OP_ADDW_I: printf("ADDW_I"); break;
        case OP_ADDW_A: printf("ADDW_A"); break;
        case OP_ADDW_R0: printf("ADDW_R0"); break;
        case OP_ADDW_R1: printf("ADDW_R1"); break;
        case OP_SUBW_I: printf("SUBW_I"); break;
        case OP_SUBW_A: printf("SUBW_A"); break;
        case OP_SUBW_R0: printf("SUBW_R0"); break;
        case OP_SUBW_R1: printf("SUBW_R1"); break;
        case OP_MULW_I: printf("MULW_I"); break;
        case OP_MULW_A: printf("MULW_A"); break;
        case OP_MULW_R0: printf("MULW_R0"); break;
        case OP_MULW_R1: printf("MULW_R1"); break;
        case OP_DIVW_I: printf("DIVW_I"); break;
        case OP_DIVW_A: printf("DIVW_A"); break;
        case OP_DIVW_R0: printf("DIVW_R0"); break;
        case OP_DIVW_R1: printf("DIVW_R1"); break;
        case OP_JMP: printf("JMP"); break;
        case OP_JIHL: printf("JIHL"); break;
        case OP_JHL: printf("JHL"); break;
        case OP_JZ: printf("JZ"); break;
        case OP_JNZ: printf("JNZ"); break;
        case OP_JC: printf("JC"); break;
        case OP_JNC: printf("JNC"); break;
        case OP_JEXT: printf("JEXT"); break;
        case OP_CMP_I: printf("CMP_I"); break;
        case OP_CMP_A: printf("CMP_A"); break;
        case OP_CMP_ML: printf("CMP_ML"); break;
        case OP_CMP_MHL: printf("CMP_MHL"); break;
        case OP_CMP_R0: printf("CMP_R0"); break;
        case OP_CMP_R1: printf("CMP_R1"); break;
        case OP_CMP_L: printf("CMP_L"); break;
        case OP_CMP_H: printf("CMP_H"); break;
        case OP_PUSH_A: printf("PUSH_A"); break;
        case OP_PUSH_I: printf("PUSH_I"); break;
        case OP_PUSH_R0: printf("PUSH_R0"); break;
        case OP_PUSH_R1: printf("PUSH_R1"); break;
        case OP_PUSH_L: printf("PUSH_L"); break;
        case OP_PUSH_H: printf("PUSH_H"); break;
        case OP_PUSH_BP: printf("PUSH_BP"); break;
        case OP_PUSH_F: printf("PUSH_F"); break;
        case OP_POP_A: printf("POP_A"); break;
        case OP_POP_IM: printf("POP_IM"); break;
        case OP_POP_R0: printf("POP_R0"); break;
        case OP_POP_R1: printf("POP_R1"); break;
        case OP_POP_L: printf("POP_L"); break;
        case OP_POP_H: printf("POP_H"); break;
        case OP_POP_BP: printf("POP_BP"); break;
        case OP_POP_F: printf("POP_F"); break;
        case OP_CALL: printf("CALL"); break;
        case OP_RET: printf("RET"); break;
        case OP_ENTER: printf("ENTER"); break;
        case OP_LEAVE: printf("LEAVE"); break;
        case OP_LOAD_BPI: printf("LOAD_BPI"); break;
        case OP_STORE_BPI: printf("STORE_BPI"); break;
        case OP_INC_HL: printf("OP_INC_HL"); break;
        case OP_DEC_HL: printf("OP_DEC_HL"); break;
        case OP_NEG_HL: printf("OP_NEG_HL"); break;
        case OP_NOT_HL: printf("OP_NOT_HL"); break;
    }
}

void print_token(void* token) {
    Token* tok = (Token*)token;
    
    switch (tok->tok) {
        case NOOP_T: printf("NOOP_T"); break;
        case HALT_T: printf("HALT_T"); break;
        case EI_T: printf("EI_T"); break;
        case DI_T: printf("DI_T"); break;
        case ET_T: printf("ET_T"); break;
        case DT_T: printf("DT_T"); break;
        case RESET_T: printf("RESET_T"); break;
        case LOAD_T: printf("LOAD_T"); break;
        case STORE_T: printf("STORE_T"); break;
        case XCH_T: printf("XCH_T"); break;
        case ADD_T: printf("ADD_T"); break;
        case ADC_T: printf("ADC_T"); break;
        case SUB_T: printf("SUB_T"); break;
        case SBC_T: printf("SBC_T"); break;
        case INC_T: printf("INC_T"); break;
        case DEC_T: printf("DEC_T"); break;
        case NEG_T: printf("NEG_T"); break;
        case NOT_T: printf("NOT_T"); break;
        case AND_T: printf("AND_T"); break;
        case OR_T: printf("OR_T"); break;
        case XOR_T: printf("XOR_T"); break;
        case SHL_T: printf("SHL_T"); break;
        case SHR_T: printf("SHR_T"); break;
        case ROL_T: printf("ROL_T"); break;
        case ROR_T: printf("ROR_T"); break;
        case ADDW_T: printf("ADDW_T"); break;
        case SUBW_T: printf("SUBW_T"); break;
        case MULW_T: printf("MULW_T"); break;
        case DIVW_T: printf("DIVW_T"); break;
        case JMP_T: printf("JMP_T"); break;
        case JZ_T: printf("JZ_T"); break;
        case JNZ_T: printf("JNZ_T"); break;
        case JC_T: printf("JC_T"); break;
        case JNC_T: printf("JNC_T"); break;
        case JEXT_T: printf("JEXT_T"); break;
        case CMP_T: printf("CMP_T"); break;
        case PUSH_T: printf("PUSH_T"); break;
        case POP_T: printf("POP_T"); break;
        case CALL_T: printf("CALL_T"); break;
        case RET_T: printf("RET_T"); break;
        case ENTER_T: printf("ENTER_T"); break;
        case LEAVE_T: printf("LEAVE_T"); break;
        case MIN_T: printf("MIN_T"); break;
        case MAX_T: printf("MAX_T"); break;
        case ACC_T: printf("ACC_T"); break;
        case R0_T: printf("R0_T"); break;
        case R1_T: printf("R1_T"); break;
        case H_T: printf("H_T"); break;
        case L_T: printf("L_T"); break;
        case HL_T: printf("HL_T"); break;
        case SP_T: printf("SP_T"); break;
        case BP_T: printf("BP_T"); break;
        case PC_T: printf("PC_T"); break;
        case FLAGS_T: printf("FLAGS_T"); break;
        case COMMA_T: printf("COMMA_T"); break;
        case QUOTED_BYTES_T: printf("QUOTED_BYTES_T"); break;
        case LABEL_REF_T: printf("LABEL_REF_T"); break;
        case LABEL_DEF_T: printf("LABEL_DEF_T"); break;
        case LABEL_IDX_T: printf("LABEL_IDX_T"); break;
        case UNKNOWN_T: printf("UNKNOWN_T"); break;
        case INTEGER_T: printf("INTEGER_T"); break;
        case BINARY_T: printf("BINARY_T"); break;
        case OCTAL_T: printf("OCTAL_T"); break;
        case HEXADECIMAL_T: printf("HEXADECIMAL_T"); break;
        case PLUS_T: printf("PLUS_T"); break;
        case MINUS_T: printf("MINUS_T"); break;
        case L_SQUARE_T: printf("L_SQUARE_T"); break;
        case R_SQUARE_T: printf("R_SQUARE_T"); break;
        case L_PAREN_T: printf("L_PAREN_T"); break;
        case R_PAREN_T: printf("R_PAREN_T"); break;
        case COMMENT_T: printf("COMMENT_T"); break;
        case L_CURLY_T: printf("L_CURLY_T"); break;
        case R_CURLY_T: printf("R_CURLY_T"); break;
    }
}

// clang-format on

static void flagDisplay(const char* name, uint8_t bit, Flags f) {
    char c;
    if (f & bit)
        c = '1';
    else
        c = '0';

    printf("    %s - %c\n", name, c);
}

void printCpu(CPU* cpu) {
    printf("----- CPU STATUS -----\n");
    printf("PC  - %u\n", PC);
    printf("ACC - %u\n", ACC);
    printf("R0  - %u\n", R0);
    printf("R1  - %u\n", R1);
    printf("H   - %u\n", H);
    printf("L   - %u\n", L);
    printf("HL  - %u\n", HL);

    printf("FLAGS\n");
    flagDisplay("CF", CF_BIT, FLAGS);
    flagDisplay("AF", AF_BIT, FLAGS);
    flagDisplay("ZF", ZF_BIT, FLAGS);
    flagDisplay("SF", SF_BIT, FLAGS);
    flagDisplay("TF", TF_BIT, FLAGS);
    flagDisplay("OF", OF_BIT, FLAGS);
    flagDisplay("IF", IF_BIT, FLAGS);

    printf("SP  - %u\n", SP);
    printf("BP  - %u\n", BP);
}

void printStack(CPU* cpu, uint8_t size) {
    size_t sp_string_idx = 1;

    printf("[");
    if (size > 0) {
        for (uint8_t i = 0; i < size - 1; ++i) {
            printf("%u, ", STACK(i));

            if (i >= SP) continue;

            if (STACK(i) < 10) {
                sp_string_idx += 3;
            } else if (STACK(i) < 100) {
                sp_string_idx += 4;
            } else {
                sp_string_idx += 5;
            }
        }
        // if (size == 255) {
        printf("%u", STACK(size - 1));
        // } else {
        //     printf("%u, ...", STACK(size - 1));
        // }
    }
    printf("]\n");

    for (size_t i = 0; i < sp_string_idx; ++i) {
        printf(" ");
    }
    printf("^ SP\n");
}
