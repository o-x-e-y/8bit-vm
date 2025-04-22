#include "headers/assembler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/cpu.h"
#include "headers/errors.h"
#include "headers/instructions.h"
#include "headers/parse_imm.h"
#include "headers/str_int_map.h"
#include "headers/tokenizer.h"

Assembler assembler;

#define HANDLE_BASIC_OP(opcode)                                                                \
    do {                                                                                       \
        Token* new_token = iter_next(&token_line);                                             \
        if (new_token == NULL || is_token_comment(new_token->tok)) {                           \
            PUSH_OP(OP_##opcode);                                                              \
        } else {                                                                               \
            printError(new_token, OP_DOESNT_USE_OPERAND_E, assembler.path, assembler.line, assembler.line_nr); \
        }                                                                                      \
    } while (0)

#define PUSH_OP(opcode)                     \
    do {                                    \
        uint8_t op = (uint8_t)opcode;       \
        push_vec(&assembler.compiled, &op); \
    } while (0)

#define PUSH_IMM8(imm)                        \
    do {                                      \
        uint8_t imm1 = (imm);                 \
        push_vec(&assembler.compiled, &imm1); \
    } while (0)

#define PUSH_IMM16(imm)                         \
    do {                                        \
        uint8_t imm1 = (uint8_t)((imm) >> 8);   \
        uint8_t imm2 = (uint8_t)((imm) & 0xFF); \
        push_vec(&assembler.compiled, &imm1);   \
        push_vec(&assembler.compiled, &imm2);   \
    } while (0)

#define PUSH_LABEL_REF(token)                                                                    \
    do {                                                                                         \
        slice_t label = (slice_t){.str = (token->substr.str + 1), .len = token->substr.len - 1}; \
        size_t idx = assembler.compiled.len;                                                     \
        LabelRef ref = (LabelRef){.label = label,                                                \
                                  .idx = idx,                                                    \
                                  .line = assembler.line,                                        \
                                  .line_nr = assembler.line_nr,                                  \
                                  .col_nr = token->char_nr + 1};                                 \
        push_vec(&assembler.label_ref_list, &ref);                                               \
        /* Push two zeros to the executable as a placeholder for the label location */           \
        PUSH_IMM16(0);                                                                           \
    } while (0)

#define PUSH_LABEL_IDX(token, offset)                                                  \
    do {                                                                               \
        LabelRef ref = (LabelRef){.label = token->substr,                              \
                                  .idx = assembler.compiled.len,                       \
                                  .line = assembler.line,                              \
                                  .line_nr = assembler.line_nr,                        \
                                  .col_nr = token->char_nr + 1};                       \
        push_vec(&assembler.label_ref_list, &ref);                                     \
        /* Push two zeros to the executable as a placeholder for the label location */ \
        PUSH_IMM16(offset);                                                            \
    } while (0)

#define PARSE_CASE(op, dst)       \
    case dst##_T:                 \
        PUSH_OP(OP_##op##_##dst); \
        break;

static void initAssembler(Assembler* assembler, slice_t filename) {
    assembler->path = filename;
    assembler->line_nr = 0;
    assembler->line = (slice_t){.str = NULL, .len = 0};
    assembler->label_ref_list = new_vec(10, sizeof(LabelRef));
    assembler->label_def_map = new_map();
    assembler->compiled = new_vec(PROGRAM_START + 100, sizeof(uint8_t));
    assembler->compiled.len = PROGRAM_START;  // first
}

static void freeAssembler(Assembler* assembler) {
    if (assembler != NULL) {
        free_map(&assembler->label_def_map);
        free_vec(&assembler->label_ref_list, NULL);
        // free_vec(&assembler->compiled, NULL);
    }
}

static inline Token* nextToken(vec_iter_t* token_line) {
    // Relies on invariant that lines contain at least one token, so this will never point to
    // invalid memory when the next token is NULL.
    Token* prev_token = token_line->ptr - token_line->elem_size;

    Token* token = iter_next(token_line);
    assert(token != prev_token);

    if (token == NULL) {
        printError(prev_token, UNEXPECTED_EOL_E, assembler.path, assembler.line, assembler.line_nr);
    }
    return token;
}

void expr_apply(uint16_t* val, ExprOperator op, uint16_t to_apply) {
    switch (op) {
        case PLUS_EOP:
            *val += to_apply;
            break;
        case MINUS_EOP:
            *val -= to_apply;
            break;
    }
}

static MemExpr parse_mem_expr(vec_iter_t token_line, TokenSymbol end) {
    Token* token;
    MemExpr expr = (MemExpr){.mode = INVALID_MM, .addr = 0};
    ExprOperator next_op = PLUS_EOP;

    token = iter_peek(&token_line);
    if (token != NULL && (token->tok == MINUS_T || token->tok == PLUS_T)) {
        iter_next(&token_line);
        next_op = (ExprOperator)token->tok;
    }

    while (true) {
        // get the expression value
        token = nextToken(&token_line);
        if (token == NULL) return (MemExpr){0};

        switch (token->tok) {
            case BINARY_T:
            case OCTAL_T:
            case INTEGER_T:
            case HEXADECIMAL_T:
                expr_apply(&expr.addr, next_op, parse_immediate(token));
                break;
            case L_T:
                if (expr.mode == L_MM || expr.mode == HL_MM) {
                    printError(token, MULTIPLE_MEMORY_E, assembler.path, assembler.line, assembler.line_nr);
                    return (MemExpr){0};
                }
                expr.mode = L_MM;
                break;
            case HL_T:
                if (expr.mode == L_MM || expr.mode == HL_MM) {
                    printError(token, MULTIPLE_MEMORY_E, assembler.path, assembler.line, assembler.line_nr);
                    return (MemExpr){0};
                }
                expr.mode = HL_MM;
                break;
            default:
                printError(token, EXPECTED_EXPR_E, assembler.path, assembler.line, assembler.line_nr);
                return (MemExpr){0};
        }

        // get the expression delim
        token = nextToken(&token_line);
        if (token == NULL) return (MemExpr){0};

        switch (token->tok) {
            case PLUS_T:
            case MINUS_T:
                next_op = (ExprOperator)token->tok;
                break;
            case R_PAREN_T:
            case R_SQUARE_T:
                if (token->tok == end) {
                    if (expr.mode == INVALID_MM) {
                        expr.mode = IMM_MM;
                    }
                    return expr;
                } else {
                    printError(token, NONMATCHING_CLOSING_PAREN_E, assembler.path, assembler.line,                          assembler.line_nr);
                    return (MemExpr){0};
                }
            default:
                printError(token, EXPECTED_EXPR_OP_E, assembler.path, assembler.line, assembler.line_nr);
                return (MemExpr){0};
        }
    }
}

static MemExpr parse_mem_addr(Token* prev_token, vec_iter_t token_line) {
    Token* token;

    switch (prev_token->tok) {
        case LABEL_IDX_T:
            token = nextToken(&token_line);
            if (token == NULL) return (MemExpr){0};

            if (token->tok == L_SQUARE_T) {
                return parse_mem_expr(token_line, R_SQUARE_T);
            }

            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);

            return (MemExpr){0};
        case L_PAREN_T:
            return parse_mem_expr(token_line, R_PAREN_T);
        default:
            assert(0 && "unreachable");
    }
}

static inline void parse_load_imm(Token* token) {
    uint8_t imm = parse_immediate(token);
    if (imm == 0) {
        PUSH_OP(OP_CLRA);
    } else {
        PUSH_OP(OP_LOAD_I);
        PUSH_IMM8(imm);
    }
}

#define PARSE_CASE_LABEL_IDX(op)                                                      \
    case LABEL_IDX_T: {                                                               \
        MemExpr expr = parse_mem_addr(token, token_line);                             \
        switch (expr.mode) {                                                          \
            case INVALID_MM:                                                          \
                assert(0 && "INVALID MEMORY ACCESS WHAT");                            \
                break;                                                                \
            case IMM_MM:                                                              \
                PUSH_OP(OP_##op##_IM);                                                \
                PUSH_LABEL_IDX(token, expr.addr);                                     \
                break;                                                                \
            case HL_MM:                                                               \
                PUSH_OP(OP_ADD_HL_I);                                                 \
                PUSH_LABEL_IDX(token, expr.addr);                                     \
                PUSH_OP(OP_##op##_MHL);                                               \
                break;                                                                \
            case L_MM:                                                                \
                assert(0 && "indexing label with just L is currently not supported"); \
        }                                                                             \
    }; break;

#define PARSE_CASE_L_PAREN(op)                            \
    case L_PAREN_T: {                                     \
        MemExpr expr = parse_mem_addr(token, token_line); \
        switch (expr.mode) {                              \
            case INVALID_MM:                              \
                break;                                    \
            case IMM_MM:                                  \
                PUSH_OP(OP_##op##_IM);                    \
                PUSH_IMM16(expr.addr);                    \
                break;                                    \
            case HL_MM:                                   \
                if (expr.addr == 0) {                     \
                    PUSH_OP(OP_##op##_MHL);               \
                } else {                                  \
                    PUSH_OP(OP_ADD_HL_I);                 \
                    PUSH_IMM16(expr.addr);                \
                    PUSH_OP(OP_##op##_MHL);               \
                }                                         \
                break;                                    \
            case L_MM:                                    \
                if (expr.addr == 0) {                     \
                    PUSH_OP(OP_##op##_ML);                \
                } else {                                  \
                    PUSH_OP(OP_ADD_L_I);                  \
                    PUSH_IMM8(expr.addr);                 \
                    PUSH_OP(OP_##op##_ML);                \
                }                                         \
                break;                                    \
        }                                                 \
    }; break;

static void parse_load(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        PARSE_CASE(LOAD, R0);
        PARSE_CASE(LOAD, R1);
        PARSE_CASE(LOAD, H);
        PARSE_CASE(LOAD, L);
        PARSE_CASE_LABEL_IDX(LOAD);
        PARSE_CASE_L_PAREN(LOAD);
        case BINARY_T:
        case OCTAL_T:
        case INTEGER_T:
        case HEXADECIMAL_T:
            parse_load_imm(token);
            break;
        case L_SQUARE_T:
            assert(0 && "LOADING FROM STACK IS NOT IMPLEMENTED YET");
            break;
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static inline void parse_store(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        PARSE_CASE(STORE, R0);
        PARSE_CASE(STORE, R1);
        PARSE_CASE(STORE, H);
        PARSE_CASE(STORE, L);
        PARSE_CASE_LABEL_IDX(STORE);
        PARSE_CASE_L_PAREN(STORE);
        case L_SQUARE_T:
            assert(0 && "STORING IN STACK IS NOT IMPLEMENTED YET");
            break;
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static inline void parse_xch(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        PARSE_CASE(XCH, R0);
        PARSE_CASE(XCH, R1);
        PARSE_CASE(XCH, H);
        PARSE_CASE(XCH, L);
        PARSE_CASE_LABEL_IDX(XCH);
        PARSE_CASE_L_PAREN(XCH);
        case BINARY_T:
        case OCTAL_T:
        case INTEGER_T:
        case HEXADECIMAL_T:
            parse_load_imm(token);
            break;
        case L_SQUARE_T:
            assert(0 && "EXCHANGING WITH STACK IS NOT IMPLEMENTED YET");
            break;
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

#define PARSE_ALU_CASE_LABEL_IDX(op)                                                  \
    case LABEL_IDX_T: {                                                               \
        MemExpr expr = parse_mem_addr(token, token_line);                             \
        switch (expr.mode) {                                                          \
            case INVALID_MM:                                                          \
                assert(0 && "INVALID MEMORY ACCESS WHAT");                            \
                break;                                                                \
            case IMM_MM:                                                              \
                PUSH_OP(OP_LOAD_HL_I);                                                \
                PUSH_LABEL_IDX(token, expr.addr);                                     \
                PUSH_OP(OP_##op##_MHL);                                               \
                break;                                                                \
            case HL_MM:                                                               \
                PUSH_OP(OP_ADD_HL_I);                                                 \
                PUSH_LABEL_IDX(token, expr.addr);                                     \
                PUSH_OP(OP_##op##_MHL);                                               \
                break;                                                                \
            case L_MM:                                                                \
                assert(0 && "indexing label with just L is currently not supported"); \
        }                                                                             \
    }; break;

#define PARSE_ALU_CASE_L_PAREN(op)                        \
    case L_PAREN_T: {                                     \
        MemExpr expr = parse_mem_addr(token, token_line); \
        switch (expr.mode) {                              \
            case INVALID_MM:                              \
                break;                                    \
            case IMM_MM:                                  \
                PUSH_OP(OP_LOAD_HL_I);                    \
                PUSH_IMM16(expr.addr);                    \
                PUSH_OP(OP_##op##_MHL);                   \
                break;                                    \
            case HL_MM:                                   \
                if (expr.addr == 0) {                     \
                    PUSH_OP(OP_##op##_MHL);               \
                } else {                                  \
                    PUSH_OP(OP_ADD_HL_I);                 \
                    PUSH_IMM16(expr.addr);                \
                    PUSH_OP(OP_##op##_MHL);               \
                }                                         \
                break;                                    \
            case L_MM:                                    \
                if (expr.addr == 0) {                     \
                    PUSH_OP(OP_##op##_ML);                \
                } else {                                  \
                    PUSH_OP(OP_ADD_L_I);                  \
                    PUSH_IMM8(expr.addr);                 \
                    PUSH_OP(OP_##op##_ML);                \
                }                                         \
                break;                                    \
        }                                                 \
    }; break;

#define PARSE_ACC_ALU(op)                                                                 \
    {                                                                                     \
        Token* token = nextToken(&token_line);                                            \
        if (token == NULL) return;                                                        \
                                                                                          \
        switch (token->tok) {                                                             \
            PARSE_CASE(op, ACC);                                                          \
            PARSE_CASE(op, R0);                                                           \
            PARSE_CASE(op, R1);                                                           \
            PARSE_CASE(op, H);                                                            \
            PARSE_CASE(op, L);                                                            \
            PARSE_ALU_CASE_LABEL_IDX(op);                                                 \
            PARSE_ALU_CASE_L_PAREN(op);                                                   \
            case BINARY_T:                                                                \
            case OCTAL_T:                                                                 \
            case INTEGER_T:                                                               \
            case HEXADECIMAL_T:                                                           \
                PUSH_OP(OP_##op##_I);                                                     \
                PUSH_IMM8(parse_immediate(token));                                        \
                break;                                                                    \
            case L_SQUARE_T:                                                              \
                assert(0 && "OPERATIONS ON STACK ARE NOT IMPLEMENTED YET");               \
                break;                                                                    \
            default:                                                                      \
                printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr); \
                break;                                                                    \
        }                                                                                 \
    }

#define PARSE_HL_ALU(op)                                                                  \
    {                                                                                     \
        Token* token = nextToken(&token_line);                                            \
        if (token == NULL) return;                                                        \
                                                                                          \
        switch (token->tok) {                                                             \
            PARSE_CASE(op, ACC);                                                          \
            PARSE_CASE(op, R0);                                                           \
            PARSE_CASE(op, R1);                                                           \
            PARSE_CASE(op, H);                                                            \
            PARSE_CASE(op, L);                                                            \
            PARSE_CASE(op, HL);                                                           \
            case L_PAREN_T:                                                               \
                assert(0 && "OPERATIONS ON MEMORY ARE NOT IMPLEMENTED YET");              \
                parse_mem_addr(token, token_line);                                        \
                break;                                                                    \
            case L_SQUARE_T:                                                              \
                assert(0 && "OPERATIONS ON STACK ARE NOT IMPLEMENTED YET");               \
                break;                                                                    \
            default:                                                                      \
                printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr); \
                break;                                                                    \
        }                                                                                 \
    }

#define PARSE_SHIFT_ALU(op)                                                               \
    {                                                                                     \
        Token* token = nextToken(&token_line);                                            \
        if (token == NULL) return;                                                        \
                                                                                          \
        switch (token->tok) {                                                             \
            PARSE_CASE(op, R0);                                                           \
            PARSE_CASE(op, R1);                                                           \
            PARSE_CASE(op, H);                                                            \
            PARSE_CASE(op, L);                                                            \
            PARSE_ALU_CASE_LABEL_IDX(op);                                                 \
            PARSE_ALU_CASE_L_PAREN(op);                                                   \
            case BINARY_T:                                                                \
            case OCTAL_T:                                                                 \
            case INTEGER_T:                                                               \
            case HEXADECIMAL_T:                                                           \
                PUSH_OP(OP_##op##_I);                                                     \
                PUSH_IMM8(parse_immediate(token));                                        \
                break;                                                                    \
            case L_SQUARE_T:                                                              \
                assert(0 && "OPERATIONS ON STACK ARE NOT IMPLEMENTED YET");               \
                break;                                                                    \
            default:                                                                      \
                printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr); \
                break;                                                                    \
        }                                                                                 \
    }

#define PARSE_WIDE(op)                                                                    \
    {                                                                                     \
        Token* token = nextToken(&token_line);                                            \
        if (token == NULL) return;                                                        \
                                                                                          \
        switch (token->tok) {                                                             \
            PARSE_CASE(op, ACC);                                                          \
            PARSE_CASE(op, R0);                                                           \
            PARSE_CASE(op, R1);                                                           \
            case BINARY_T:                                                                \
            case OCTAL_T:                                                                 \
            case INTEGER_T:                                                               \
            case HEXADECIMAL_T:                                                           \
                PUSH_OP(OP_##op##_I);                                                     \
                PUSH_IMM16(parse_immediate(token));                                       \
                break;                                                                    \
            case L_PAREN_T:                                                               \
                assert(0 && "OPERATIONS ON MEMORY ARE NOT IMPLEMENTED YET");              \
                parse_mem_addr(token, token_line);                                        \
                break;                                                                    \
            case L_SQUARE_T:                                                              \
                assert(0 && "OPERATIONS ON STACK ARE NOT IMPLEMENTED YET");               \
                break;                                                                    \
            default:                                                                      \
                printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr); \
                break;                                                                    \
        }                                                                                 \
    }

#define PARSE_JMP(op)                                                                     \
    {                                                                                     \
        Token* token = nextToken(&token_line);                                            \
        if (token == NULL) return;                                                        \
                                                                                          \
        switch (token->tok) {                                                             \
            case BINARY_T:                                                                \
            case OCTAL_T:                                                                 \
            case INTEGER_T:                                                               \
            case HEXADECIMAL_T:                                                           \
                PUSH_OP(OP_##op);                                                         \
                PUSH_IMM16(parse_immediate(token));                                       \
                break;                                                                    \
            case LABEL_REF_T: {                                                           \
                PUSH_OP(OP_##op);                                                         \
                PUSH_LABEL_REF(token);                                                    \
                break;                                                                    \
            }                                                                             \
            default:                                                                      \
                printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr); \
                break;                                                                    \
        }                                                                                 \
    }

static inline void parse_jext(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;
    uint8_t flags;

    switch (token->tok) {
        case BINARY_T:
        case OCTAL_T:
        case INTEGER_T:
        case HEXADECIMAL_T:
            flags = parse_immediate(token);

            token = nextToken(&token_line);
            if (token == NULL) return;

            switch (token->tok) {
                case COMMA_T:
                    token = nextToken(&token_line);
                    if (token == NULL) return;

                    switch (token->tok) {
                        case BINARY_T:
                        case OCTAL_T:
                        case INTEGER_T:
                        case HEXADECIMAL_T:
                            PUSH_OP(OP_JEXT);
                            PUSH_IMM8(flags);
                            PUSH_IMM16(parse_immediate(token));
                            break;
                        case LABEL_REF_T: {
                            PUSH_OP(OP_JEXT);
                            PUSH_IMM8(flags);
                            PUSH_LABEL_REF(token);
                            break;
                        }
                        default:
                            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line,
                                       assembler.line_nr);
                            break;
                    }
                    break;
                default:
                    printError(token, EXPECTED_COMMA_E, assembler.path, assembler.line, assembler.line_nr);
                    break;
            }
            break;

        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static inline void parse_push(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        PARSE_CASE(PUSH, ACC);
        PARSE_CASE(PUSH, R0);
        PARSE_CASE(PUSH, R1);
        PARSE_CASE(PUSH, H);
        PARSE_CASE(PUSH, L);
        PARSE_CASE(PUSH, BP);
        PARSE_CASE(PUSH, FLAGS);
        case BINARY_T:
        case OCTAL_T:
        case INTEGER_T:
        case HEXADECIMAL_T:
            PUSH_OP(OP_PUSH_I);
            PUSH_IMM8(parse_immediate(token));
            break;
        case L_PAREN_T:
            assert(0 && "PUSHING FROM MEMORY IS NOT IMPLEMENTED YET");
            parse_mem_addr(token, token_line);
            break;
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static inline void parse_pop(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        PARSE_CASE(POP, ACC);
        PARSE_CASE(POP, R0);
        PARSE_CASE(POP, R1);
        PARSE_CASE(POP, H);
        PARSE_CASE(POP, L);
        PARSE_CASE(POP, BP);
        PARSE_CASE(POP, FLAGS);
        case L_PAREN_T:
            assert(0 && "POPPING TO MEMORY IS NOT IMPLEMENTED YET");
            parse_mem_addr(token, token_line);
            break;
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static inline void parse_call(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        // I don't think it makes sense to 'call' an absolute memory address, but perhaps it
        // should nontheless be legal
        //
        // case BINARY_T:
        // case OCTAL_T:
        // case INTEGER_T:
        // case HEXADECIMAL_T:
        //     PUSH_OP(OP_CALL);
        //     PUSH_IMM16(parse_immediate(token));
        //     break;
        case LABEL_REF_T: {
            PUSH_OP(OP_CALL);
            PUSH_LABEL_REF(token);
            break;
        }
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static inline void parse_enter(vec_iter_t token_line) {
    Token* token = nextToken(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        case BINARY_T:
        case OCTAL_T:
        case INTEGER_T:
        case HEXADECIMAL_T:
            PUSH_OP(OP_CALL);
            PUSH_IMM8(parse_immediate(token));
            break;
        default:
            printError(token, UNEXPECTED_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            break;
    }
}

static void assembleLinePass1(TokenLine* line) {
    assembler.line_nr = line->line_nr;
    assembler.line = line->substr;

    vec_iter_t token_line = iter_from_vec(&line->tokens);

    Token* token = iter_next(&token_line);
    if (token == NULL) return;

    switch (token->tok) {
        case NOOP_T:
            HANDLE_BASIC_OP(NOOP);
            break;
        case HALT_T:
            HANDLE_BASIC_OP(HALT);
            break;
        case EI_T:
            HANDLE_BASIC_OP(EI);
            break;
        case DI_T:
            HANDLE_BASIC_OP(DI);
            break;
        case ET_T:
            HANDLE_BASIC_OP(ET);
            break;
        case DT_T:
            HANDLE_BASIC_OP(DT);
            break;
        case RESET_T:
            HANDLE_BASIC_OP(RESET);
            break;
        case RET_T:
            HANDLE_BASIC_OP(RET);
            break;
        case LEAVE_T:
            HANDLE_BASIC_OP(LEAVE);
            break;
        case LOAD_T:
            parse_load(token_line);
            break;
        case STORE_T:
            parse_store(token_line);
            break;
        case XCH_T:
            parse_xch(token_line);
            break;
        case ADD_T:
            PARSE_ACC_ALU(ADD);
            break;
        case ADC_T:
            PARSE_ACC_ALU(ADC);
            break;
        case SUB_T:
            PARSE_ACC_ALU(SUB);
            break;
        case SBC_T:
            PARSE_ACC_ALU(SBC);
            break;
        case INC_T:
            PARSE_HL_ALU(INC);
            break;
        case DEC_T:
            PARSE_HL_ALU(DEC);
            break;
        case NEG_T:
            PARSE_HL_ALU(NEG);
            break;
        case NOT_T:
            PARSE_HL_ALU(NOT);
            break;
        case AND_T:
            PARSE_ACC_ALU(AND);
            break;
        case OR_T:
            PARSE_ACC_ALU(OR);
            break;
        case XOR_T:
            PARSE_ACC_ALU(XOR);
            break;
        case SHL_T:
            PARSE_SHIFT_ALU(SHL);
            break;
        case SHR_T:
            PARSE_SHIFT_ALU(SHR);
            break;
        case ROL_T:
            PARSE_SHIFT_ALU(ROL);
            break;
        case ROR_T:
            PARSE_SHIFT_ALU(ROR);
            break;
        case ADDW_T:
            PARSE_WIDE(ADDW);
            break;
        case SUBW_T:
            PARSE_WIDE(SUBW);
            break;
        case MULW_T:
            PARSE_WIDE(MULW);
            break;
        case DIVW_T:
            PARSE_WIDE(DIVW);
            break;
        case JMP_T:
            PARSE_JMP(JMP);
            break;
        case JZ_T:
            PARSE_JMP(JZ);
            break;
        case JNZ_T:
            PARSE_JMP(JNZ);
            break;
        case JC_T:
            PARSE_JMP(JC);
            break;
        case JNC_T:
            PARSE_JMP(JNC);
            break;
        case JEXT_T:
            parse_jext(token_line);
            break;
        case CMP_T:
            PARSE_ACC_ALU(CMP);
            break;
        case PUSH_T:
            parse_push(token_line);
            break;
        case POP_T:
            parse_pop(token_line);
            break;
        case CALL_T:
            parse_call(token_line);
            break;
        case ENTER_T:
            parse_enter(token_line);
            break;
        case MIN_T:
            PARSE_SHIFT_ALU(MIN);
            break;
        case MAX_T:
            PARSE_SHIFT_ALU(MAX);
            break;
        case UNKNOWN_T:
            printError(token, UNKNOWN_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            while ((token = iter_next(&token_line))) {
                if (is_token_unknown(token->tok))
                    printError(token, UNKNOWN_TOKEN_E, assembler.path, assembler.line, assembler.line_nr);
            }
            break;
        case INTEGER_T:
        case BINARY_T:
        case OCTAL_T:
        case HEXADECIMAL_T:
            do {
                uint16_t imm = parse_immediate(token);
                if (imm > 255)
                    printWarning(token, U8_OVERFLOW_W, assembler.path, assembler.line, assembler.line_nr);

                PUSH_IMM8(imm);
            } while ((token = iter_next(&token_line)) && is_token_immediate(token->tok));
            break;
        case QUOTED_BYTES_T:
            // the substr contains quotes on both sides, so we need some weird indexing
            for (size_t i = 1; i < token->substr.len - 1; ++i) {
                PUSH_IMM8(token->substr.str[i]);
            }
            break;
        case LABEL_DEF_T: {
            // label defs end with : which is included in the token substr
            slice_t label = (slice_t){.str = token->substr.str, .len = token->substr.len - 1};
            insert_map(&assembler.label_def_map, label, assembler.compiled.len);
            break;
        }
        case COMMENT_T:
            break;
        // I could use `default` here but I want clang to tell me if I forgot anything
        case ACC_T:
        case R0_T:
        case R1_T:
        case H_T:
        case L_T:
        case HL_T:
        case SP_T:
        case BP_T:
        case PC_T:
        case FLAGS_T:
        case COMMA_T:
        case LABEL_REF_T:
        case LABEL_IDX_T:
        case PLUS_T:
        case MINUS_T:
        case L_SQUARE_T:
        case R_SQUARE_T:
        case L_PAREN_T:
        case R_PAREN_T:
        case L_CURLY_T:
        case R_CURLY_T:
            printError(token, EXPECTED_OPERATOR_E, assembler.path, assembler.line, assembler.line_nr);
    }
}

/// Compile the assembled tokens into an executable with placeholder zeroes in place of labeled
/// jumps and memory access. Those will be filled out in pass 2.
static void assemblePass1(TokenLines lines) {
    vec_iter_t token_lines = iter_from_vec(&lines.lines);
    TokenLine* line;

    while ((line = iter_next(&token_lines))) {
        assembleLinePass1(line);
    }

    // always add a HALT right at the end, might not keep this
    PUSH_OP(OP_HALT);
}

/// Takes the compiled program currently in the Assembler and works out the jump locations
static void assemblePass2() {
    LabelRef* ref;
    vec_iter_t label_refs = iter_from_vec(&assembler.label_ref_list);

    while ((ref = iter_next(&label_refs))) {
        size_t* idx = get_map(&assembler.label_def_map, ref->label);

        if (idx == NULL) {
            Token tok = (Token){.tok = LABEL_REF_T, .char_nr = ref->col_nr, .substr = ref->label};
            printError(&tok, UNDEFINED_LABEL_E, assembler.path, ref->line, ref->line_nr);
        } else {
            size_t idx_jmp_from_high = ref->idx;     // label location high byte
            size_t idx_jmp_from_low = ref->idx + 1;  // low byte

            // These might be non-zero if working with an indexed label
            uint8_t base_high = *(uint8_t*)get_vec(&assembler.compiled, idx_jmp_from_high);
            uint8_t base_low = *(uint8_t*)get_vec(&assembler.compiled, idx_jmp_from_low);

            uint8_t jmp_to_high = base_high + (uint8_t)((*idx >> 8) & 0xFF);
            uint8_t jmp_to_low = base_low + (uint8_t)(*idx & 0xFF);

            // set bytes of label location
            set_vec(&assembler.compiled, &jmp_to_high, idx_jmp_from_high);
            set_vec(&assembler.compiled, &jmp_to_low, idx_jmp_from_low);
        }
    }
}

Executable assemble(slice_t program, slice_t filename) {
    initAssembler(&assembler, filename);

    TokenLines lines = tokenizeProgram(program);

    assemblePass1(lines);

    assemblePass2();

    Executable exec =
        (Executable){.executable = assembler.compiled.ptr, .size = assembler.compiled.len};

    freeAssembler(&assembler);
    freeTokenLines(&lines);

    return exec;
}
