#include "headers/errors.h"

#include <stdint.h>

#include "headers/parse_imm.h"
#include "stdio.h"

#define INDICATE_TOKEN_256                                                                         \
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" \
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" \
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
#define SPACES_256                                                                                 \
    "                                                                                            " \
    "                                                                                            " \
    "                                                                        "

static void printErrorMsg(ParserError err, Token* tok) {
    switch (err) {
        case UNKNOWN_TOKEN_E:
            printf("unknown token `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case UNEXPECTED_TOKEN_E:
        case OP_DOESNT_USE_OPERAND_E:
            printf("unexpected token `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case UNEXPECTED_EOL_E:
            printf("unexpected end of line");
            break;
        case EXPECTED_COMMA_E:
            printf("expected comma, found `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case EXPECTED_OPERATOR_E:
            printf("expected operator, found `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case UNDEFINED_LABEL_E:
            printf("undefined label `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case NONMATCHING_CLOSING_PAREN_E:
            printf("closing parenthesis `%.*s` does not match opening parenthesis",
                   (int)tok->substr.len, tok->substr.str);
            break;
        case EXPECTED_EXPR_OP_E:
            printf("expected +, - or a closing parenthesis, found `%.*s`", (int)tok->substr.len,
                   tok->substr.str);
            break;
        case EXPECTED_EXPR_E:
            printf("expected H, L or an integer, found `%.*s`", (int)tok->substr.len,
                   tok->substr.str);
            break;
        case MULTIPLE_MEMORY_E:
            printf("encountered more than one of `L` or `HL` in memory expression");
            break;
            // case EXPECTED_PLUS_MINUS_E:
            //     printf("expected plus or minus");
            //     break;
    }
}

static void printErrorHelpMsg(ParserError err) {
    switch (err) {
        case UNKNOWN_TOKEN_E:
            printf("you probably meant something else");
            break;
        case UNEXPECTED_TOKEN_E:
            printf("another token was expected here");
            break;
        case OP_DOESNT_USE_OPERAND_E:
            printf("no operand was expected for this instruction");
            break;
        case UNEXPECTED_EOL_E:
            printf("expected token after this point");
            break;
        case EXPECTED_COMMA_E:
            printf("operands should be separated by a comma, add one here");
            break;
        case EXPECTED_OPERATOR_E:
            printf("lines should start with an operator such as `ADD`");
            break;
        case UNDEFINED_LABEL_E:
            printf("every label reference should have a matching label definition");
            break;
        case NONMATCHING_CLOSING_PAREN_E:
            printf("every memory expression should have matching left and right brackets");
            break;
        case EXPECTED_EXPR_OP_E:
            printf("expression operands should be delimited by + or -");
            break;
        case EXPECTED_EXPR_E:
            printf("expression members consist of H, L or integers");
            break;
        case MULTIPLE_MEMORY_E:
            printf("only one of `L` or `HL` is allowed");
            // case EXPECTED_PLUS_MINUS_E:
            //     printf("expected plus or minus");
            //     break;
    }
}

static void alignErrorHelpMsg(size_t char_nr, size_t len) {
    printf("\e[0;34m     |  %.*s", (int)char_nr, SPACES_256);
    printf("\e[0;31m%.*s help: ", (int)len, INDICATE_TOKEN_256);
}

static void alignWarningHelpMsg(size_t char_nr, size_t len) {
    printf("\e[0;34m     |  %.*s", (int)char_nr, SPACES_256);
    printf("\e[0;33m%.*s help: ", (int)len, INDICATE_TOKEN_256);
}

static void alignWarningHelpMsgNewline(size_t char_nr, size_t len) {
    printf("\e[0;34m     |  %.*s", (int)char_nr, SPACES_256);
    printf("\e[0;33m%.*s       ", (int)len, SPACES_256);
}

static void printWarningMsg(ParserWarning warning, Token* tok) {
    switch (warning) {
        case U8_OVERFLOW_W:
            printf(
                "the provided literal value `%.*s` does not fit inside a u8 but is interpreted "
                "that way",
                (int)tok->substr.len, tok->substr.str);
            break;
    }
}

static void printWarningHelpMsg(ParserWarning warning, Token* tok) {
    switch (warning) {
        case U8_OVERFLOW_W: {
            uint16_t imm = parse_immediate(tok);
            uint8_t high = (imm >> 8) & 0xFF;
            uint8_t low = imm & 0xFF;
            printf("if you meant to save a u16 integer literal, you must split it up\n");
            alignWarningHelpMsgNewline(tok->char_nr, tok->substr.len);
            printf("into two bytes: `%u %u`", high, low);
            break;
        }
    }
}

void printError(Token* tok, ParserError error, slice_t line, size_t line_nr) {
    printf("\e[1;31mERROR\e[1;37m: ");

    printErrorMsg(error, tok);

    printf("\n    \e[0;34m-->\e[0m file_path/file.c:%lu:%lu\n", line_nr, tok->char_nr);
    printf("\e[0;34m     |\n");
    printf("%4lu |  \e[0m%.*s\n", line_nr, (int)line.len, line.str);

    alignErrorHelpMsg(tok->char_nr, tok->substr.len);
    printErrorHelpMsg(error);

    printf("\n\e[0;34m     |\n\n");
    printf("\e[0m");
}

void printWarning(Token* tok, ParserWarning warning, slice_t line, size_t line_nr) {
    printf("\e[1;33mWARNING\e[1;37m: ");

    printWarningMsg(warning, tok);

    printf("\n    \e[0;34m-->\e[0m file_path/file.c:%lu:%lu\n", line_nr, tok->char_nr);
    printf("\e[0;34m     |\n");
    printf("%4lu |  \e[0m%.*s\n", line_nr, (int)line.len, line.str);

    alignWarningHelpMsg(tok->char_nr, tok->substr.len);
    printWarningHelpMsg(warning, tok);

    printf("\n\e[0;34m     |\n\n");
    printf("\e[0m");
}
