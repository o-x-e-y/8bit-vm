#ifndef __OXEY_SLICE_H
#define __OXEY_SLICE_H

#include "ostring.h"
#include "string.h"

#define static_slice(str) from_cstr_slice(str, strlen(str))

typedef struct {
    const size_t len;
    const char* str;
} slice_t;

slice_t from_cstr_slice(const char* str, size_t len);
slice_t from_str_slice(const string_t* str);
slice_t copy_slice(const slice_t* str);

size_t len_slice(const slice_t* slice);

char get_slice(const slice_t* slice, size_t index);
char first_slice(const slice_t* slice);
char last_slice(const slice_t* slice);

bool eq_slice(const slice_t* lhs, const slice_t* rhs);
bool eq_str_slice(const slice_t* lhs, const string_t* rhs);
bool eq_cstr_slice(const slice_t* slice, const char* s, size_t len);

void print_slice(const slice_t* slice);
void debug_slice(const slice_t* slice);

str_iter_t iter_from_slice(const slice_t* slice);

#endif
