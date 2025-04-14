#ifndef __OXEY_ERRORS_H__
#define __OXEY_ERRORS_H__

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
} ParserError;

typedef enum {
    U8_OVERFLOW_W,
} ParserWarning;

void printWarning(Token* tok, ParserWarning warning, slice_t line, size_t line_nr);
void printError(Token* tok, ParserError error, slice_t line, size_t line_nr);

#endif
