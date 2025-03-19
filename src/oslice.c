#include "headers/oslice.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

slice_t from_cstr_slice(const char* str, size_t len) { return (slice_t){.len = len, .str = str}; }

slice_t from_str_slice(const string_t* str) {
    assert(str != NULL);
    return (slice_t){.len = str->len, .str = str->str};
}

slice_t copy_slice(const slice_t* slice) {
    assert(slice != NULL);
    return (slice_t){.len = slice->len, .str = slice->str};
}

size_t len_slice(const slice_t* slice) {
    assert(slice != NULL);
    return slice->len;
}

char get_slice(const slice_t* slice, size_t index) {
    assert(slice != NULL);

    if (index >= slice->len) {
        return 0;
    } else {
        return slice->str[index];
    }
}

char first_slice(const slice_t* slice) {
    assert(slice != NULL);

    if (slice->len == 0) {
        return 0;
    } else {
        return slice->str[0];
    }
}
char last_slice(const slice_t* slice) {
    assert(slice != NULL);

    if (slice->len == 0) {
        return 0;
    } else {
        return slice->str[slice->len - 1];
    }
}

bool inline eq_cstr_slice(const slice_t* slice, const char* s, size_t len) {
    assert(slice != NULL);
    assert(s != NULL);

    if (slice->len != len) {
        return false;
    }

    return strncmp(slice->str, s, len) == 0;
}

bool eq_slice(const slice_t* lhs, const slice_t* rhs) {
    assert(lhs != NULL);
    assert(rhs != NULL);

    return eq_cstr_slice(lhs, rhs->str, rhs->len);
}

bool eq_str_slice(const slice_t* lhs, const string_t* rhs) {
    assert(lhs != NULL);
    assert(rhs != NULL);

    return eq_cstr_slice(lhs, rhs->str, rhs->len);
}

void print_slice(const slice_t* slice) {
    assert(slice != NULL);

    printf("\"");

    if (slice->len > 0) {
        for (size_t i = 0; i < slice->len; ++i) {
            printf("%c", get_slice(slice, i));
        }
    }

    printf("\"");
}
void debug_slice(const slice_t* slice) {
    assert(slice != NULL);

    printf("slice_t {    \n    len: %zu\n    ptr: %p\n}\n", slice->len, slice->str);
}

str_iter_t iter_from_slice(const slice_t* slice) {
    assert(slice != NULL);

    char* start = (char*)slice->str;
    const char* end = slice->str + (slice->len - 1) * sizeof(char);

    return (str_iter_t){
        .ptr = start,
        .end = end,
    };
}
