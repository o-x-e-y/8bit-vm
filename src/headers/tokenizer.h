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
    JS_T,
    JNS_T,
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
    QUOTED_BYTES_T,
    LABEL_REF_T,
    LABEL_DEF_T,
    LABEL_IDX_T,
    COMMENT_T,
    UNKNOWN_T,

    INTEGER_T,
    BINARY_T,
    OCTAL_T,
    HEXADECIMAL_T,
    PLUS_T,
    MINUS_T,
    EQUALS_T,
    L_SQUARE_T,
    R_SQUARE_T,
    L_PAREN_T,
    R_PAREN_T,
    L_CURLY_T,
    R_CURLY_T,
} TokenSymbol;

typedef struct {
    TokenSymbol tok;
    slice_t substr;
    size_t char_nr;
} Token;

typedef struct {
    vec_t tokens;
    slice_t substr;
    size_t line_nr;
} TokenLine;

typedef struct {
    vec_t lines;
} TokenLines;

void freeTokenLine(void* tokenLine);
void freeTokenLines(void* tokenLines);
TokenSymbol tokenizeSymbol(str_iter_t* iter);
TokenLine tokenizeLine(str_iter_t* iter, size_t line_nr);
TokenLines tokenizeProgram(slice_t program);

bool is_token_op(TokenSymbol token);
bool is_token_register(TokenSymbol token);
bool is_token_addr(TokenSymbol token);
bool is_token_comma(TokenSymbol token);
bool is_token_comment(TokenSymbol token);
bool is_token_label_ref(TokenSymbol token);
bool is_token_label_def(TokenSymbol token);
bool is_token_label(TokenSymbol token);
bool is_token_immediate(TokenSymbol token);
bool is_token_unknown(TokenSymbol token);

#endif
