#include "headers/parse_imm.h"
#include "stdio.h"
#include "stdlib.h"

size_t parse_immediate(Token* token) {
    switch (token->tok) {
        case BINARY_T:
            return strtol(&token->substr.str[2], NULL, 2);
        case OCTAL_T:
            return strtol(&token->substr.str[2], NULL, 8);
        case INTEGER_T:
            return strtol(token->substr.str, NULL, 10);
        case HEXADECIMAL_T:
            return strtol(&token->substr.str[2], NULL, 16);
        default:
            printf("Not an immediate - compiler error because it should be unreachable!!");
            exit(1);
    }
}
