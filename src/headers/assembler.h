#ifndef __OXEY_CCE_ASSEMBLER_H
#define __OXEY_CCE_ASSEMBLER_H

#include <stdint.h>

#include "oslice.h"
#include "str_int_map.h"
#include "tokenizer.h"

typedef enum {
    UNKNOWN_TOKEN_E,
    UNEXPECTED_TOKEN_E,
    OP_DOESNT_USE_OPERAND_E,
    UNEXPECTED_EOL_E,
    EXPECTED_COMMA_E,
    EXPECTED_OPERATOR_E,
    UNDEFINED_LABEL_E,
    NONMATCHING_CLOSING_PAREN_E,
    EXPECTED_EXPR_OP_E,
    EXPECTED_EXPR_E,
    MULTIPLE_MEMORY_E,
    // EXPECTED_PLUS_MINUS_E,
    // UNEXPECTED_OPERATION_E,
    // UNEXPECTED_OPERAND_E
} ParserError;

typedef enum {
    U8_OVERFLOW_W,
} ParserWarning;

typedef enum {
    PLUS_EOP = PLUS_T,
    MINUS_EOP = MINUS_T,
} ExprOperator;

typedef enum {
    INVALID_MM,
    IMM_MM,
    HL_MM,
    L_MM,
} MemMode;

typedef struct MemAddr {
    MemMode mode;
    uint16_t addr;
} MemExpr;

typedef struct {
    slice_t label;
    size_t idx;
    slice_t line;
    size_t line_nr;
    size_t col_nr;
} LabelRef;

typedef struct Assembler {
    size_t line_nr;
    slice_t line;
    map_t label_def_map;
    vec_t label_ref_list;
    vec_t compiled;
} Assembler;

typedef struct Executable {
    uint8_t* executable;
    size_t size;
} Executable;

Executable assemble(slice_t program);

#endif
