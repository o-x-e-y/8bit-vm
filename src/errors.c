#include "headers/errors.h"

#include <stdint.h>

#include "headers/oslice.h"
#include "headers/parse_imm.h"
#include "headers/tokenizer.h"
#include "stdio.h"

#define INDICATE_TOKEN_256                                                                         \
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" \
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" \
    "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
#define SPACES_256                                                                                 \
    "                                                                                            " \
    "                                                                                            " \
    "                                                                        "
// clang-format off
#define RGB(r, g, b)            "\033[38;2;" #r ";" #g ";" #b "m"
#define GRUVBOX_RED             RGB(251, 73, 52)
#define GRUVBOX_GREEN           RGB(184, 187, 38)
#define GRUVBOX_YELLOW          RGB(250, 189, 47)
#define GRUVBOX_BLUE            RGB(131, 165, 152)
#define GRUVBOX_MAGENTA	        RGB(211, 134, 155)
#define GRUVBOX_CYAN            RGB(142, 192, 124)
#define GRUVBOX_ORANGE          RGB(254, 128, 25)
#define GRUVBOX_GREY            RGB(146, 131, 116)
#define GRUVBOX_DARK_RED        RGB(176, 51, 36)
#define GRUVBOX_DARK_GREEN      RGB(129, 131, 27)
#define GRUVBOX_DARK_YELLOW     RGB(175, 132, 33)
#define GRUVBOX_DARK_BLUE       RGB(92, 116, 106)
#define GRUVBOX_DARK_MAGENTA    RGB(148, 94, 109)
#define GRUVBOX_DARK_CYAN       RGB(99, 134, 87)
#define GRUVBOX_DARK_ORANGE     RGB(178, 90, 18)
#define GRUVBOX_DARK_GREY       RGB(102, 92, 81)
#define RESET                   "\033[0m"
#define BOLD                    "\033[1m"

// clang-format on

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
        case INVALID_STACK_INDEX_E:
            printf("invalid stack index `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case INVALID_MEMORY_INDEX_E:
            printf("invalid memory index `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case INVALID_MEMORY_IDX_INDEX_E:
            printf("invalid indexed memory index `%.*s`", (int)tok->substr.len, tok->substr.str);
            break;
        case INVALID_OPERAND_TYPE_E:
            printf("invalid operand type");
            break;
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
            break;
        case INVALID_STACK_INDEX_E:
            printf("the stack must be indexed by BP, i.e. {BP + 1}");
            break;
        case INVALID_MEMORY_INDEX_E:
            printf("memory can be indexed by HL, L or an immediate value");
            break;
        case INVALID_MEMORY_IDX_INDEX_E:
            printf("indexed memory can only be indexed by HL or an immediate value");
            break;
        case INVALID_OPERAND_TYPE_E:
            printf("some operations only take specific operands");
            break;
    }
}

static void alignErrorHelpMsg(size_t char_nr, size_t len) {
    printf(GRUVBOX_BLUE "     |  %.*s", (int)char_nr, SPACES_256);
    printf(GRUVBOX_RED "%.*s help: ", (int)len, INDICATE_TOKEN_256);
}

static void alignWarningHelpMsg(size_t char_nr, size_t len) {
    printf(GRUVBOX_BLUE "     |  %.*s", (int)char_nr, SPACES_256);
    printf(GRUVBOX_YELLOW "%.*s help: ", (int)len, INDICATE_TOKEN_256);
}

static void alignWarningHelpMsgNewline(size_t char_nr, size_t len) {
    printf(GRUVBOX_BLUE "     |  %.*s", (int)char_nr, SPACES_256);
    printf(GRUVBOX_YELLOW "%.*s       ", (int)len, SPACES_256);
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

void printHighlightedLine(Assembler* assembler, bool darken) {
    str_iter_t line_iter = iter_from_slice(assembler->line);

    printf(RESET);

    TokenLine line = tokenizeLine(&line_iter, assembler->line_nr);

    vec_iter_t token_iter = iter_from_vec(&line.tokens);
    Token* token;

    printf(RESET);

    size_t last_char_nr = 0;
    size_t last_len = 0;

    while ((token = iter_next(&token_iter))) {
        size_t spaces = (token->char_nr - last_char_nr) - last_len;
        printf("%.*s", (int)spaces, SPACES_256);
        last_char_nr = token->char_nr;
        last_len = token->substr.len;

        if (darken) {
            if (is_token_op(token->tok)) {
                printf(GRUVBOX_DARK_RED);
            } else if (is_token_register(token->tok)) {
                printf(GRUVBOX_DARK_YELLOW);
            } else if (is_token_addr(token->tok)) {
                printf(GRUVBOX_DARK_ORANGE);
            } else if (is_token_immediate(token->tok)) {
                printf(GRUVBOX_DARK_MAGENTA);
            } else if (is_token_label(token->tok)) {
                printf(GRUVBOX_DARK_GREEN);
            } else if (is_token_comment(token->tok)) {
                printf(GRUVBOX_DARK_GREY);
            } else {
                printf(RESET);
            }
        } else {
            if (is_token_op(token->tok)) {
                printf(GRUVBOX_RED);
            } else if (is_token_register(token->tok)) {
                printf(GRUVBOX_YELLOW);
            } else if (is_token_addr(token->tok)) {
                printf(GRUVBOX_ORANGE);
            } else if (is_token_immediate(token->tok)) {
                printf(GRUVBOX_MAGENTA);
            } else if (is_token_label(token->tok)) {
                printf(GRUVBOX_GREEN);
            } else if (is_token_comment(token->tok)) {
                printf(GRUVBOX_GREY);
            } else {
                printf(RESET);
            }
        }

        printf("%.*s" RESET, (int)token->substr.len, token->substr.str);
    }
}

static void printNumberedHighlightedLine(Assembler* assembler, bool darken) {
    printf(GRUVBOX_BLUE "%4lu |  " RESET, assembler->line_nr);
    printHighlightedLine(assembler, darken);
    printf(RESET "\n");
}

#define STRINGIFY_EXPR(expr) #expr

static TokenLine* findTokenLineWithLineNr(vec_iter_t token_line_iter, size_t line_nr) {
    TokenLine* line;

    while ((line = iter_next(&token_line_iter))) {
        if (line->line_nr == line_nr) {
            return line;
        }
    }
    return NULL;
}

static void printNumberedHighlightedLines(Assembler* assembler, size_t n_back) {
    size_t old_line_nr = assembler->line_nr;
    slice_t old_line = assembler->line;
    vec_iter_t token_line_iter = iter_from_vec(&assembler->token_lines);
    TokenLine* line;

    for (size_t i = assembler->line_nr - (n_back - 1); i <= old_line_nr; ++i) {
        void* old_token_ptr = token_line_iter.ptr;

        if ((line = findTokenLineWithLineNr(token_line_iter, i))) {
            assembler->line_nr = line->line_nr;
            assembler->line = line->substr;
            bool darken = old_line_nr == line->line_nr ? false : true;
            printNumberedHighlightedLine(assembler, darken);
        } else {
            assembler->line_nr = i;
            assembler->line = (slice_t){.len = 0, .str = ""};
            printNumberedHighlightedLine(assembler, false);
        }

        // optimalization: since we only look forward, only start looking from the last found idx
        // we might skip over a following index so we jump ahead to the last one used
        token_line_iter.ptr = old_token_ptr;
    }

    assembler->line_nr = old_line_nr;
    assembler->line = old_line;
}

void printError(Token* tok, ParserError error, Assembler* assembler) {
    printf(GRUVBOX_RED BOLD "ERROR" RESET BOLD ": ");

    printErrorMsg(error, tok);

    printf("\n    " GRUVBOX_BLUE "-->" RESET " %.*s:%lu:%lu\n", (int)assembler->path.len,
           assembler->path.str, assembler->line_nr, tok->char_nr);
    printf(GRUVBOX_BLUE "     |\n");
    printNumberedHighlightedLines(assembler, 3);

    alignErrorHelpMsg(tok->char_nr, tok->substr.len);
    printErrorHelpMsg(error);

    printf(GRUVBOX_BLUE "\n     |\n\n" RESET);
}

void printWarning(Token* tok, ParserWarning warning, Assembler* assembler) {
    printf(GRUVBOX_YELLOW BOLD "WARNING" RESET BOLD ": ");

    printWarningMsg(warning, tok);

    printf("\n    " GRUVBOX_BLUE "-->" RESET " %.*s:%lu:%lu\n", (int)assembler->path.len,
           assembler->path.str, assembler->line_nr, tok->char_nr);
    printf(GRUVBOX_BLUE "     |\n");
    printNumberedHighlightedLines(assembler, 3);

    alignWarningHelpMsg(tok->char_nr, tok->substr.len);
    printWarningHelpMsg(warning, tok);

    printf("\n" GRUVBOX_BLUE "     |\n\n" RESET);
}
