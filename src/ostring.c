#include "headers/ostring.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/util.h"

static void __grow_str(string_t* str);

string_t new_str(size_t item_capacity) {
    return (string_t){
        .capacity = item_capacity,
        .len = 0,
        .str = malloc(item_capacity * sizeof(char)),
    };
}

string_t from_cstr_str(const char* str, size_t len) {
    char* ptr = malloc(len);

    strncpy(ptr, str, len);

    return (string_t){
        .capacity = len,
        .len = len,
        .str = ptr,
    };
}

void free_str(string_t* str) {
    if (str) {
        free(str->str);
    }

    str = NULL;
}

const char* as_cstr(string_t* str) {
    push_str(str, 0);
    str->len--;
    return str->str;
}

size_t len_str(const string_t* str) { return str->len; }
size_t capacity_str(const string_t* str) { return str->capacity; }

char get_str(const string_t* str, size_t index) {
    if (index >= str->capacity) {
        return 0;
    } else {
        return str->str[index];
    }
}

char first_str(const string_t* str) {
    if (str->len == 0) {
        return 0;
    } else {
        return str->str[0];
    }
}

char last_str(const string_t* str) {
    if (str->len == 0) {
        return 0;
    } else {
        return str->str[str->len - 1];
    }
}

string_t clone_str(const string_t* str) {
    char* ptr = malloc(str->len);

    strncpy(ptr, str->str, str->len);

    return (string_t){
        .capacity = str->len,
        .len = str->len,
        .str = ptr,
    };
}

bool eq_str(const string_t* lhs, string_t* rhs) {
    if (lhs->len != rhs->len) {
        return false;
    }

    return strncmp(lhs->str, rhs->str, lhs->len) == 0;
}

bool eq_cstr(const string_t* str, const char* s, size_t len) {
    if (str->len != len) {
        return false;
    }

    return strncmp(str->str, s, len) == 0;
}

void push_str(string_t* str, char c) {
    assert(str != NULL);

    str->str[str->len] = c;
    str->len++;

    __grow_str(str);
}

void push_cstr_str(string_t* str, const char* s, size_t len) {
    assert(str != NULL);
    assert(s != NULL);

    if (str->capacity <= str->len + len) {
        size_t new_capacity = min(str->len * 2, str->capacity) + len;

        str->str = realloc(str->str, new_capacity * sizeof(char));
        str->capacity = new_capacity;
    }

    memcpy(str->str + str->len, s, len * sizeof(char));

    str->len += len;
}

void push_str_str(string_t* str, string_t* app) { push_cstr_str(str, app->str, app->len); }

void set_str(string_t* str, char c, size_t index) {
    assert(str != NULL);
    assert(index < str->len);

    str->str[index] = c;
}

char pop_str(string_t* str) {
    assert(str != NULL);

    if (str->len == 0) {
        return 0;
    } else {
        str->len--;

        return str->str[str->len];
    }
}

void clear_str(string_t* str) { str->len = 0; }

void insert_str(string_t* str, char c, size_t index) {
    assert(str != NULL);
    assert(index <= str->len);

    if (index == str->len) {
        push_str(str, c);
        return;
    }

    str->len++;

    __grow_str(str);

    size_t offset = index * sizeof(char);

    char* src_ptr = str->str + offset;
    char* dst_ptr = str->str + offset + sizeof(char);
    size_t copy_len = (str->len - index) * sizeof(char);

    memmove(dst_ptr, src_ptr, copy_len);
    src_ptr[index] = c;
}

void remove_str(string_t* str, size_t index) {
    assert(str != NULL);

    if (index < str->len) {
        size_t offset = index * sizeof(char);
        char* src_ptr = str->str + offset + sizeof(char);
        char* dst_ptr = str->str + offset;
        size_t copy_len = (str->len - index) * sizeof(char);

        memmove(dst_ptr, src_ptr, copy_len);
        str->len--;
    }
}

void resize_str(string_t* str, size_t len) {
    assert(str != NULL);

    if (len == str->len) {
        return;
    }

    size_t new_size = len * sizeof(char);

    str->str = realloc(str->str, new_size);
    str->len = min(str->len, len);
    str->capacity = len;
}

void reserve_str(string_t* str, size_t chars) {
    assert(str != NULL);

    if (chars <= str->len) {
        return;
    } else if (chars < str->len * 2) {
        chars = str->len * 2;
    }

    resize_str(str, chars);
}

void reverse_str(string_t* str) { reverse_cstr(str->str, str->len); }

void reverse_cstr(char* str, size_t len) {
    if (len < 2) {
        return;
    }

    size_t last = len - 1;

    for (size_t i = 0; i < len / 2; ++i) {
        char help = str[i];
        str[i] = str[last - i];
        str[last - i] = help;
    }
}

void map_str(string_t* str, char map(char)) {
    assert(str != NULL);

    for (size_t i = 0; i < str->len; ++i) {
        str->str[i] = map(str->str[i]);
    }
}

void print_str(const string_t* str) {
    assert(str != NULL);

    printf("\"");

    if (str->len > 0) {
        for (size_t i = 0; i < str->len; ++i) {
            printf("%c", get_str(str, i));
        }
    }

    printf("\"");
}

void debug_str(const string_t* str) {
    assert(str != NULL);

    printf("string_t {    \n    capacity: %zu\n    len: %zu\n    ptr: %p\n}\n", str->capacity,
           str->len, str->str);
}

static void __grow_str(string_t* str) {
    if (str->len >= str->capacity) {
        size_t new_capacity = (str->capacity + 1) * 2;

        char* tmp = realloc(str->str, new_capacity * sizeof(char));
        if (tmp) {
            str->str = tmp;
            str->capacity = new_capacity;
        } else {
            exit(-1);
        }
    }
}

str_iter_t iter_from_cstr(const char* str, size_t len) {
    assert(str != NULL);

    return (str_iter_t){
        .ptr = (char*)(str),
        .end = (char*)(str + (len * sizeof(char))),
    };
}

str_iter_t iter_from_str(const string_t* str) {
    assert(str != NULL);

    const char* end = str->str + (str->len - 1) * sizeof(char);

    return (str_iter_t){
        .ptr = str->str,
        .end = (char*)end,
    };
}

char str_iter_peek(const str_iter_t* iter) {
    assert(iter != NULL);

    if (iter->ptr <= iter->end) {
        return *iter->ptr;
    } else {
        return 0;
    }
}

char str_iter_peek_n(const str_iter_t* iter, size_t n) {
    assert(iter != NULL);

    if (iter->ptr + n <= iter->end) {
        return *(iter->ptr + n);
    } else {
        return 0;
    }
}

char str_iter_next(str_iter_t* iter) {
    assert(iter != NULL);

    if (iter->ptr <= iter->end) {
        char tmp = iter->ptr[0];

        iter->ptr++;

        return tmp;
    } else {
        return 0;
    }
}

void str_iter_skip(str_iter_t* iter, size_t skip) {
    assert(iter != NULL);

    if (iter->ptr + skip <= iter->end) {
        iter->ptr += skip;
    } else {
        iter->ptr = (char*)(iter->end + 1);
    }
}

void str_iter_skip_whitespace(str_iter_t* iter) {
    char curr;

    while (isspace(curr = str_iter_next(iter))) {
        continue;
    }

    iter->ptr--;
}
