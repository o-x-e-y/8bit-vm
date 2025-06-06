#ifndef __OXEY_CCE_ASSEMBLER_H
#define __OXEY_CCE_ASSEMBLER_H

#include <stdint.h>

#include "oslice.h"
#include "ovec.h"
#include "str_int_map.h"
#include "tokenizer.h"

typedef enum {
    PLUS_EOP = PLUS_T,
    MINUS_EOP = MINUS_T,
} ExprOperator;

typedef enum {
    INVALID_MM,
    IMM_MM,
    HL_MM,
    L_MM,
    BP_MM,
} MemMode;

typedef struct MemExpr {
    MemMode mode;
    uint16_t addr;
    Token* mode_token;
} MemExpr;

typedef struct {
    slice_t label;
    size_t idx;
    slice_t line;
    size_t line_nr;
    size_t col_nr;
} LabelRef;

typedef struct Assembler {
    slice_t path;
    vec_t token_lines;
    size_t line_nr;
    slice_t line;
    si_map_t label_def_map;
    vec_t label_ref_list;
    vec_t compiled;
} Assembler;

typedef struct Executable {
    uint8_t* executable;
    size_t size;
} Executable;

Executable assemble(slice_t program, slice_t path);

#endif
