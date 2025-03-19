#include "headers/oslice.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

slice_t from_cstr_slice(const char* str, size_t len) { return (slice_t){.len = len, .str = str}; }

slice_t from_str_slice(string_t str) { return (slice_t){.len = str.len, .str = str.str}; }

slice_t copy_slice(slice_t slice) { return (slice_t){.len = slice.len, .str = slice.str}; }

size_t inline len_slice(slice_t slice) { return slice.len; }

char inline get_slice(slice_t slice, size_t index) {
    if (index >= slice.len) {
        return 0;
    } else {
        return slice.str[index];
    }
}

char first_slice(slice_t slice) {
    if (slice.len == 0) {
        return 0;
    } else {
        return slice.str[0];
    }
}
char last_slice(slice_t slice) {
    if (slice.len == 0) {
        return 0;
    } else {
        return slice.str[slice.len - 1];
    }
}

bool inline eq_cstr_slice(slice_t slice, const char* s, size_t len) {
    assert(s != NULL);

    if (slice.len != len) {
        return false;
    }

    return strncmp(slice.str, s, len) == 0;
}

bool eq_slice(slice_t lhs, slice_t rhs) { return eq_cstr_slice(lhs, rhs.str, rhs.len); }

bool eq_str_slice(slice_t lhs, string_t rhs) { return eq_cstr_slice(lhs, rhs.str, rhs.len); }

void print_slice(slice_t slice) {
    printf("\"");

    if (slice.len > 0) {
        for (size_t i = 0; i < slice.len; ++i) {
            printf("%c", get_slice(slice, i));
        }
    }

    printf("\"");
}
void debug_slice(slice_t slice) {
    printf("slice_t {    \n    len: %zu\n    ptr: %p\n}\n", slice.len, slice.str);
}

str_iter_t iter_from_slice(slice_t slice) {
    char* start = (char*)slice.str;
    const char* end = slice.str + (slice.len - 1) * sizeof(char);

    return (str_iter_t){
        .ptr = start,
        .end = end,
    };
}
