#ifndef __OXEY_CCE_TOKENIZER_H
#define __OXEY_CCE_TOKENIZER_H

#include "oslice.h"
#include "ostring.h"
#include "ovec.h"

typedef enum AssemblyToken {
    NOOP_T,
    HALT_T,
    EI_T,
    DI_T,
    ET_T,
    DT_T,
    CLRA_T,
    RESET_T,
    LOAD_T,
    STORE_T,
    XCH_T,
    ADD_T,
    ADC_T,
    SUB_T,
    SBC_T,
    INC_T,
    DEC_T,
    NEG_T,
    NOT_T,
    AND_T,
    OR_T,
    XOR_T,
    SHL_T,
    SHR_T,
    ROL_T,
    ROR_T,
    ADDW_T,
    SUBW_T,
    MULW_T,
    DIVW_T,
    JMP_T,
    JZ_T,
    JNZ_T,
    JC_T,
    JNC_T,
    JEXT_T,
    CMP_T,
    PUSH_T,
    POP_T,
    CALL_T,
    RET_T,
    ENTER_T,
    LEAVE_T,
    MIN_T,
    MAX_T,
    ACC_T,
    R0_T,
    R1_T,
    H_T,
    L_T,
    HL_T,
    SP_T,
    BP_T,
    PC_T,
    FLAGS_T,

    COMMA_T,
    LABEL_REF_T,
    LABEL_DEF_T,
    COMMENT_T,
    UNKNOWN_T,

    INTEGER_T,
    BINARY_T,
    OCTAL_T,
    HEXADECIMAL_T,
    PLUS_T,
    MINUS_T,
    L_SQUARE_T,
    R_SQUARE_T,
    L_PAREN_T,
    R_PAREN_T,
    // L_CURLY_T,
    // R_CURLY_T,
} AssemblyToken;

typedef struct {
    AssemblyToken tok;
    slice_t substr;
} Token;

typedef struct {
    vec_t tokens;
} TokenLine;

typedef struct {
    vec_t lines;
} ProgramLines;

AssemblyToken tokenizeSymbol(str_iter_t* iter);
TokenLine tokenizeLine(str_iter_t* iter);
ProgramLines tokenizeProgram(slice_t program);

#endif