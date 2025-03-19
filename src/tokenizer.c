#include "headers/tokenizer.h"

#include <ctype.h>

#include "headers/ostring.h"
#include "headers/ovec.h"

static void str_iter_skip_space(str_iter_t* iter) {
    char p;
    while ((p = str_iter_peek(iter)) && p == ' ') {
        str_iter_next(iter);
    }
}

static void str_iter_skip_comment(str_iter_t* iter) {
    char p;
    while ((p = str_iter_peek(iter)) && p != '\n') {
        str_iter_next(iter);
    }
}

static void str_iter_skip_number(str_iter_t* iter) {
    char p;
    if ((p = tolower(str_iter_peek(iter))) && (p == 'b' || p == 'o' || p == 'x')) {
        str_iter_next(iter);
    }
    while ((p = str_iter_peek(iter)) && isxdigit(p)) {
        str_iter_next(iter);
    }
}

static void str_iter_skip_label(str_iter_t* iter) {
    char p;
    while ((p = tolower(str_iter_peek(iter))) && (islower(p) || p == '_')) {
        str_iter_next(iter);
    }
}

AssemblyToken tokenizeSymbol(str_iter_t* iter) {
    AssemblyToken error = UNKNOWN_T;

    /// Partially generated using trie-gen: https://www.nongnu.org/trie-gen/
    switch (str_iter_next(iter)) {
        case '(':
            return L_PAREN_T;
        case ')':
            return R_PAREN_T;
        case '+':
            return PLUS_T;
        case '-':
            return MINUS_T;
        case '[':
            return L_SQUARE_T;
        case ']':
            return R_SQUARE_T;
        case '{':
            return L_CURLY_T;
        case '}':
            return R_CURLY_T;
        case ',':
            return COMMA_T;
        case ';':
            str_iter_skip_comment(iter);
            return COMMENT_T;
        case '.':
            str_iter_skip_label(iter);
            return LABEL_REF_T;
        case 'A':
            switch (str_iter_next(iter)) {
                case 'C':
                    if (str_iter_next(iter) == 'C' && str_iter_peek(iter) == ' ') return ACC_T;
                    return error;
                case 'D':
                    switch (str_iter_next(iter)) {
                        case 'C':
                            if (str_iter_peek(iter) == ' ') return ADC_T;
                            return error;
                        case 'D':
                            if (str_iter_peek(iter) == ' ') return ADD_T;
                            if (str_iter_next(iter) == 'W' && str_iter_peek(iter) == ' ')
                                return ADDW_T;
                    }
                    return error;
                case 'N':
                    if (str_iter_next(iter) == 'D' && str_iter_peek(iter) == ' ') return AND_T;
            }
            return error;
        case 'B':
            if (str_iter_next(iter) == 'P') return BP_T;
            return error;
        case 'C':
            switch (str_iter_next(iter)) {
                case 'A':
                    if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'L' &&
                        str_iter_peek(iter) == ' ')
                        return CALL_T;
                    return error;
                case 'L':
                    if (str_iter_next(iter) == 'R' && str_iter_next(iter) == 'A' &&
                        str_iter_peek(iter) == ' ')
                        return CLRA_T;
                    return error;
                case 'M':
                    if (str_iter_next(iter) == 'P' && str_iter_peek(iter) == ' ') return CMP_T;
            }
            return error;
        case 'D':
            switch (str_iter_next(iter)) {
                case 'E':
                    if (str_iter_next(iter) == 'C' && str_iter_peek(iter) == ' ') return DEC_T;
                    return error;
                case 'I':
                    if (str_iter_peek(iter) == ' ') return DI_T;
                    if (str_iter_next(iter) == 'V' && str_iter_next(iter) == 'W' &&
                        str_iter_peek(iter) == ' ')
                        return DIVW_T;
                    return error;
                case 'T':
                    if (str_iter_peek(iter) == ' ') return DT_T;
            }
            return error;
        case 'E':
            switch (str_iter_next(iter)) {
                case 'I':
                    if (str_iter_peek(iter) == ' ') return EI_T;
                    return error;
                case 'N':
                    if (str_iter_next(iter) == 'T' && str_iter_next(iter) == 'E' &&
                        str_iter_next(iter) == 'R' && str_iter_peek(iter) == ' ')
                        return ENTER_T;
                    return error;
                case 'T':
                    if (str_iter_peek(iter) == ' ') return ET_T;
            }
            return error;
        case 'F':
            if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'A' &&
                str_iter_next(iter) == 'G' && str_iter_next(iter) == 'S' &&
                str_iter_peek(iter) == ' ')
                return FLAGS_T;
            return error;
        case 'H':
            switch (str_iter_next(iter)) {
                case 'A':
                    if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'T' &&
                        str_iter_peek(iter) == ' ')
                        return HALT_T;
                    return error;
                case 'L':
                    return HL_T;
                default:
                    iter->ptr--;
                    return H_T;
            }
            return error;
        case 'I':
            if (str_iter_next(iter) == 'N' && str_iter_next(iter) == 'C' &&
                str_iter_peek(iter) == ' ')
                return INC_T;
            return error;
        case 'J':
            switch (str_iter_next(iter)) {
                case 'C':
                    if (str_iter_peek(iter) == ' ') return JC_T;
                    return error;
                case 'E':
                    if (str_iter_next(iter) == 'X' && str_iter_next(iter) == 'T' &&
                        str_iter_peek(iter) == ' ')
                        return JEXT_T;
                    return error;
                case 'N':
                    switch (str_iter_next(iter)) {
                        case 'C':
                            if (str_iter_peek(iter) == ' ') return JNC_T;
                            return error;
                        case 'Z':
                            if (str_iter_peek(iter) == ' ') return JNZ_T;
                            return error;
                    }
                    return error;
                case 'M':
                    if (str_iter_next(iter) == 'P' && str_iter_peek(iter) == ' ') return JMP_T;
                    return error;
                case 'Z':
                    if (str_iter_peek(iter) == ' ') return JZ_T;
                    return error;
            }
            return error;
        case 'L':
            switch (str_iter_next(iter)) {
                case 'E':
                    if (str_iter_next(iter) == 'A' && str_iter_next(iter) == 'V' &&
                        str_iter_next(iter) == 'E' && str_iter_peek(iter) == ' ')
                        return LEAVE_T;
                    return error;
                case 'O':
                    if (str_iter_next(iter) == 'A' && str_iter_next(iter) == 'D' &&
                        str_iter_peek(iter) == ' ')
                        return LOAD_T;
                default:
                    iter->ptr--;
                    return L_T;
            }
            return error;
        case 'M':
            switch (str_iter_next(iter)) {
                case 'A':
                    if (str_iter_next(iter) == 'X' && str_iter_peek(iter) == ' ') return MAX_T;
                    return error;
                case 'I':
                    if (str_iter_next(iter) == 'N' && str_iter_peek(iter) == ' ') return MIN_T;
                    return error;
                case 'U':
                    if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'W' &&
                        str_iter_peek(iter) == ' ')
                        return MULW_T;
            }
            return error;
        case 'N':
            switch (str_iter_next(iter)) {
                case 'E':
                    if (str_iter_next(iter) == 'G' && str_iter_peek(iter) == ' ') return NEG_T;
                    return error;
                case 'O':
                    switch (str_iter_next(iter)) {
                        case 'O':
                            if (str_iter_next(iter) == 'P' && str_iter_peek(iter) == ' ')
                                return NOOP_T;
                            return error;
                        case 'T':
                            if (str_iter_peek(iter) == ' ') return NOT_T;
                    }
            }
            return error;
        case 'O':
            if (str_iter_next(iter) == 'R' && str_iter_peek(iter) == ' ') return OR_T;
            return error;
        case 'P':
            switch (str_iter_next(iter)) {
                case 'C':
                    return PC_T;
                case 'O':
                    if (str_iter_next(iter) == 'P' && str_iter_peek(iter) == ' ') return POP_T;
                    return error;
                case 'U':
                    if (str_iter_next(iter) == 'S' && str_iter_next(iter) == 'H' &&
                        str_iter_peek(iter) == ' ')
                        return PUSH_T;
            }
            return error;
        case 'R':
            switch (str_iter_next(iter)) {
                case '0':
                    return R0_T;
                case '1':
                    return R1_T;
                case 'E':
                    switch (str_iter_next(iter)) {
                        case 'S':
                            if (str_iter_next(iter) == 'E' && str_iter_next(iter) == 'T' &&
                                str_iter_peek(iter) == ' ')
                                return RESET_T;
                            return error;
                        case 'T':
                            if (str_iter_peek(iter) == ' ') return RET_T;
                    }
                    return error;
                case 'O':
                    switch (str_iter_next(iter)) {
                        case 'L':
                            if (str_iter_peek(iter) == ' ') return ROL_T;
                            return error;
                        case 'R':
                            if (str_iter_peek(iter) == ' ') return ROR_T;
                    }
            }
            return error;
        case 'S':
            switch (str_iter_next(iter)) {
                case 'B':
                    if (str_iter_next(iter) == 'C' && str_iter_peek(iter) == ' ') return SBC_T;
                    return error;
                case 'H':
                    switch (str_iter_next(iter)) {
                        case 'L':
                            if (str_iter_peek(iter) == ' ') return SHL_T;
                            return error;
                        case 'R':
                            if (str_iter_peek(iter) == ' ') return SHR_T;
                    }
                    return error;
                case 'P':
                    return SP_T;
                case 'T':
                    if (str_iter_next(iter) == 'O' && str_iter_next(iter) == 'R' &&
                        str_iter_next(iter) == 'E' && str_iter_peek(iter) == ' ')
                        return STORE_T;
                    return error;
                case 'U':
                    if (str_iter_next(iter) == 'B') {
                        if (str_iter_peek(iter) == ' ') return SUB_T;
                        if (str_iter_next(iter) == 'W' && str_iter_peek(iter) == ' ') return SUBW_T;
                    }
            }
            return error;
        case 'X':
            switch (str_iter_next(iter)) {
                case 'C':
                    if (str_iter_next(iter) == 'H' && str_iter_peek(iter) == ' ') return XCH_T;
                    return error;
                case 'O':
                    if (str_iter_next(iter) == 'R' && str_iter_peek(iter) == ' ') return XOR_T;
            }
            return error;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            switch (tolower(str_iter_peek(iter))) {
                case 'b':
                    str_iter_skip_number(iter);
                    return BINARY_T;
                case 'o':
                    str_iter_skip_number(iter);
                    return OCTAL_T;
                case 'x':
                    str_iter_skip_number(iter);
                    return HEXADECIMAL_T;
                default:
                    str_iter_skip_number(iter);
                    return INTEGER_T;
            }
            return error;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '_':
            str_iter_skip_label(iter);
            return LABEL_DEF_T;
    }
    return error;
}

TokenLine tokenizeLine(str_iter_t* iter) {
    str_iter_skip_space(iter);

    if (str_iter_peek(iter) == '\n') {
        str_iter_next(iter);
        return (TokenLine){.tokens = new_vec(0, sizeof(Token))};
    }

    vec_t res = new_vec(10, sizeof(Token));

    char p;
    while ((p = str_iter_peek(iter)) && p != '\n') {
        str_iter_skip_space(iter);

        char* start = iter->ptr;

        AssemblyToken atok = tokenizeSymbol(iter);

        char* end = iter->ptr;
        slice_t substr = from_cstr_slice(start, (size_t)(end - start));

        Token tok = (Token){.substr = substr, .tok = atok};
        push_vec(&res, &tok);

        str_iter_skip_space(iter);  // skip spaces until potential newline
    }

    return (TokenLine){.tokens = res};
}

ProgramLines tokenizeProgram(slice_t program) {
    vec_t res = new_vec(10, sizeof(TokenLine));
    char p;

    str_iter_t iter = iter_from_slice(&program);

    while (str_iter_peek(&iter)) {
        str_iter_skip_whitespace(&iter);

        TokenLine line = tokenizeLine(&iter);

        if (line.tokens.len > 0) {
            push_vec(&res, &line);
        }
    }

    return (ProgramLines){.lines = res};
}