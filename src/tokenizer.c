#include "headers/tokenizer.h"

#include <ctype.h>
#include <stdio.h>

#include "headers/ostring.h"
#include "headers/ovec.h"

static bool istokdelim(char c) {
    switch (c) {
        case ' ':
        case '\0':
        case '\t':
        case '\n':
        case EOF:
            return true;
        default:
            return false;
    }
}

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
    while ((p = tolower(str_iter_peek(iter))) && (islower(p) || isdigit(p) || p == '_')) {
        str_iter_next(iter);
    }
}

static void str_iter_skip_until_doublequotes(str_iter_t* iter) {
    char p;
    while ((p = str_iter_peek(iter)) && p != '"') {
        str_iter_next(iter);
    }
    str_iter_next(iter); // skip final doublequote
}

static TokenSymbol unknown(str_iter_t* iter) {
    char p;
    while ((p = str_iter_peek(iter)) && isalnum(p)) {
        str_iter_next(iter);
    }

    return UNKNOWN_T;
}

TokenSymbol tokenizeSymbol(str_iter_t* iter) {
    // TokenSymbol error = UNKNOWN_T;

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
        case '"':
            str_iter_skip_until_doublequotes(iter);
            return QUOTED_BYTES_T;
        case ';':
            str_iter_skip_comment(iter);
            return COMMENT_T;
        case '.':
            str_iter_skip_label(iter);
            return LABEL_REF_T;
        case 'A':
            switch (str_iter_next(iter)) {
                case 'C':
                    if (str_iter_next(iter) == 'C' && istokdelim(str_iter_peek(iter))) return ACC_T;
                    return unknown(iter);
                case 'D':
                    switch (str_iter_next(iter)) {
                        case 'C':
                            if (istokdelim(str_iter_peek(iter))) return ADC_T;
                            return unknown(iter);
                        case 'D':
                            if (istokdelim(str_iter_peek(iter))) return ADD_T;
                            if (str_iter_next(iter) == 'W' && istokdelim(str_iter_peek(iter)))
                                return ADDW_T;
                    }
                    return unknown(iter);
                case 'N':
                    if (str_iter_next(iter) == 'D' && istokdelim(str_iter_peek(iter))) return AND_T;
            }
            return unknown(iter);
        case 'B':
            if (str_iter_next(iter) == 'P') return BP_T;
            return unknown(iter);
        case 'C':
            switch (str_iter_next(iter)) {
                case 'A':
                    if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'L' &&
                        istokdelim(str_iter_peek(iter)))
                        return CALL_T;
                    return unknown(iter);
                case 'M':
                    if (str_iter_next(iter) == 'P' && istokdelim(str_iter_peek(iter))) return CMP_T;
            }
            return unknown(iter);
        case 'D':
            switch (str_iter_next(iter)) {
                case 'E':
                    if (str_iter_next(iter) == 'C' && istokdelim(str_iter_peek(iter))) return DEC_T;
                    return unknown(iter);
                case 'I':
                    if (istokdelim(str_iter_peek(iter))) return DI_T;
                    if (str_iter_next(iter) == 'V' && str_iter_next(iter) == 'W' &&
                        istokdelim(str_iter_peek(iter)))
                        return DIVW_T;
                    return unknown(iter);
                case 'T':
                    if (istokdelim(str_iter_peek(iter))) return DT_T;
            }
            return unknown(iter);
        case 'E':
            switch (str_iter_next(iter)) {
                case 'I':
                    if (istokdelim(str_iter_peek(iter))) return EI_T;
                    return unknown(iter);
                case 'N':
                    if (str_iter_next(iter) == 'T' && str_iter_next(iter) == 'E' &&
                        str_iter_next(iter) == 'R' && istokdelim(str_iter_peek(iter)))
                        return ENTER_T;
                    return unknown(iter);
                case 'T':
                    if (istokdelim(str_iter_peek(iter))) return ET_T;
            }
            return unknown(iter);
        case 'F':
            if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'A' &&
                str_iter_next(iter) == 'G' && str_iter_next(iter) == 'S' &&
                istokdelim(str_iter_peek(iter)))
                return FLAGS_T;
            return unknown(iter);
        case 'H':
            switch (str_iter_next(iter)) {
                case 'A':
                    if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'T' &&
                        !isalnum(str_iter_peek(iter)))
                        return HALT_T;
                    return unknown(iter);
                case 'L':
                    return HL_T;
                default:
                    iter->ptr--;
                    return H_T;
            }
            return unknown(iter);
        case 'I':
            if (str_iter_next(iter) == 'N' && str_iter_next(iter) == 'C' &&
                istokdelim(str_iter_peek(iter)))
                return INC_T;
            return unknown(iter);
        case 'J':
            switch (str_iter_next(iter)) {
                case 'C':
                    if (istokdelim(str_iter_peek(iter))) return JC_T;
                    return unknown(iter);
                case 'E':
                    if (str_iter_next(iter) == 'X' && str_iter_next(iter) == 'T' &&
                        istokdelim(str_iter_peek(iter)))
                        return JEXT_T;
                    return unknown(iter);
                case 'N':
                    switch (str_iter_next(iter)) {
                        case 'C':
                            if (istokdelim(str_iter_peek(iter))) return JNC_T;
                            return unknown(iter);
                        case 'Z':
                            if (istokdelim(str_iter_peek(iter))) return JNZ_T;
                            return unknown(iter);
                    }
                    return unknown(iter);
                case 'M':
                    if (str_iter_next(iter) == 'P' && istokdelim(str_iter_peek(iter))) return JMP_T;
                    return unknown(iter);
                case 'Z':
                    if (istokdelim(str_iter_peek(iter))) return JZ_T;
                    return unknown(iter);
            }
            return unknown(iter);
        case 'L':
            switch (str_iter_next(iter)) {
                case 'E':
                    if (str_iter_next(iter) == 'A' && str_iter_next(iter) == 'V' &&
                        str_iter_next(iter) == 'E' && istokdelim(str_iter_peek(iter)))
                        return LEAVE_T;
                    return unknown(iter);
                case 'O':
                    if (str_iter_next(iter) == 'A' && str_iter_next(iter) == 'D' &&
                        istokdelim(str_iter_peek(iter)))
                        return LOAD_T;
                default:
                    iter->ptr--;
                    return L_T;
            }
            return unknown(iter);
        case 'M':
            switch (str_iter_next(iter)) {
                case 'A':
                    if (str_iter_next(iter) == 'X' && istokdelim(str_iter_peek(iter))) return MAX_T;
                    return unknown(iter);
                case 'I':
                    if (str_iter_next(iter) == 'N' && istokdelim(str_iter_peek(iter))) return MIN_T;
                    return unknown(iter);
                case 'U':
                    if (str_iter_next(iter) == 'L' && str_iter_next(iter) == 'W' &&
                        istokdelim(str_iter_peek(iter)))
                        return MULW_T;
            }
            return unknown(iter);
        case 'N':
            switch (str_iter_next(iter)) {
                case 'E':
                    if (str_iter_next(iter) == 'G' && istokdelim(str_iter_peek(iter))) return NEG_T;
                    return unknown(iter);
                case 'O':
                    switch (str_iter_next(iter)) {
                        case 'O':
                            if (str_iter_next(iter) == 'P' && istokdelim(str_iter_peek(iter)))
                                return NOOP_T;
                            return unknown(iter);
                        case 'T':
                            if (istokdelim(str_iter_peek(iter))) return NOT_T;
                    }
            }
            return unknown(iter);
        case 'O':
            if (str_iter_next(iter) == 'R' && istokdelim(str_iter_peek(iter))) return OR_T;
            return unknown(iter);
        case 'P':
            switch (str_iter_next(iter)) {
                case 'C':
                    return PC_T;
                case 'O':
                    if (str_iter_next(iter) == 'P' && istokdelim(str_iter_peek(iter))) return POP_T;
                    return unknown(iter);
                case 'U':
                    if (str_iter_next(iter) == 'S' && str_iter_next(iter) == 'H' &&
                        istokdelim(str_iter_peek(iter)))
                        return PUSH_T;
            }
            return unknown(iter);
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
                                istokdelim(str_iter_peek(iter)))
                                return RESET_T;
                            return unknown(iter);
                        case 'T':
                            if (istokdelim(str_iter_peek(iter))) return RET_T;
                    }
                    return unknown(iter);
                case 'O':
                    switch (str_iter_next(iter)) {
                        case 'L':
                            if (istokdelim(str_iter_peek(iter))) return ROL_T;
                            return unknown(iter);
                        case 'R':
                            if (istokdelim(str_iter_peek(iter))) return ROR_T;
                    }
            }
            return unknown(iter);
        case 'S':
            switch (str_iter_next(iter)) {
                case 'B':
                    if (str_iter_next(iter) == 'C' && istokdelim(str_iter_peek(iter))) return SBC_T;
                    return unknown(iter);
                case 'H':
                    switch (str_iter_next(iter)) {
                        case 'L':
                            if (istokdelim(str_iter_peek(iter))) return SHL_T;
                            return unknown(iter);
                        case 'R':
                            if (istokdelim(str_iter_peek(iter))) return SHR_T;
                    }
                    return unknown(iter);
                case 'P':
                    return SP_T;
                case 'T':
                    if (str_iter_next(iter) == 'O' && str_iter_next(iter) == 'R' &&
                        str_iter_next(iter) == 'E' && istokdelim(str_iter_peek(iter)))
                        return STORE_T;
                    return unknown(iter);
                case 'U':
                    if (str_iter_next(iter) == 'B') {
                        if (istokdelim(str_iter_peek(iter))) return SUB_T;
                        if (str_iter_next(iter) == 'W' && istokdelim(str_iter_peek(iter)))
                            return SUBW_T;
                    }
            }
            return unknown(iter);
        case 'X':
            switch (str_iter_next(iter)) {
                case 'C':
                    if (str_iter_next(iter) == 'H' && istokdelim(str_iter_peek(iter))) return XCH_T;
                    return unknown(iter);
                case 'O':
                    if (str_iter_next(iter) == 'R' && istokdelim(str_iter_peek(iter))) return XOR_T;
            }
            return unknown(iter);
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
            return unknown(iter);
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
            if (str_iter_peek(iter) == ':') {
                str_iter_next(iter);
                return LABEL_DEF_T;
            }
            if (str_iter_peek(iter) == '[') {
                return LABEL_IDX_T;
            }
            return unknown(iter);
    }
    return unknown(iter);
}

TokenLine tokenizeLine(str_iter_t* iter, size_t lineNr) {
    str_iter_skip_space(iter);

    if (str_iter_peek(iter) == '\n') {
        str_iter_next(iter);
        return (TokenLine){0};
    }

    vec_t res = new_vec(10, sizeof(Token));

    char p;
    while ((p = str_iter_peek(iter)) && p != '\n') {
        str_iter_skip_space(iter);

        const char* start = iter->ptr;

        TokenSymbol symbol = tokenizeSymbol(iter);

        const char* end = iter->ptr;
        const slice_t substr = from_cstr_slice(start, (size_t)(end - start));
        const size_t char_nr = (size_t)(start - line_start);

        Token tok = (Token){.substr = substr, .tok = symbol, .char_nr = char_nr};
        push_vec(&res, &tok);

        str_iter_skip_space(iter);  // skip spaces until potential newline
    }

    slice_t substr = from_cstr_slice(line_start, (size_t)(iter->ptr - line_start));

    return (TokenLine){.tokens = res, .substr = substr, .line_nr = line_nr};
}

TokenLines tokenizeProgram(slice_t program) {
    vec_t res = new_vec(10, sizeof(TokenLine));
    size_t lineNr = 0;

    str_iter_t iter = iter_from_slice(program);

    while (str_iter_peek(&iter)) {
        TokenLine line = tokenizeLine(&iter, lineNr);

        if (line.tokens.len > 0) {
            push_vec(&res, &line);
        }
        ++lineNr;
    }

    return (TokenLines){.lines = res};
}

void freeTokenLine(void* tokenLine) {
    if (tokenLine != NULL) {
        TokenLine* line = tokenLine;
        free_vec(&line->tokens, NULL);
    }
}

void freeTokenLines(void* tokenLines) {
    if (tokenLines != NULL) {
        TokenLines* t = tokenLines;
        free_vec(&t->lines, freeTokenLine);
    }
}
