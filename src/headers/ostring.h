#ifndef __OXEY_STRING_H
#define __OXEY_STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define static_str(str) from_cstr_str(str, strlen(str))

typedef struct {
    size_t len;
    size_t capacity;

    char* str;
} string_t;

typedef struct {
    char* ptr;
    const char* end;
} str_iter_t;

string_t new_str(size_t capacity);
string_t from_cstr_str(const char* str, size_t len);
void free_str(string_t* str);
const char* as_cstr(string_t* str);

size_t len_str(const string_t* str);
size_t capacity_str(const string_t* str);

char get_str(const string_t* str, size_t index);
char first_str(const string_t* str);
char last_str(const string_t* str);

string_t clone_str(const string_t* str);
bool eq_str(const string_t* lhs, string_t* rhs);
bool eq_cstr(const string_t* str, const char* s, size_t len);

void push_str(string_t* str, char c);
void push_cstr_str(string_t* str, const char* s, size_t len);
void push_str_str(string_t* str, string_t* app);
char pop_str(string_t* str);

void set_str(string_t* str, char s, size_t index);

void insert_str(string_t* str, char s, size_t index);
void remove_str(string_t* str, size_t index);
void clear_str(string_t* str);

void reserve_str(string_t* str, size_t sents);
void resize_str(string_t* str, size_t len);

void reverse_str(string_t* str);
void reverse_cstr(char* str, size_t len);

void print_str(const string_t* str);
void debug_str(const string_t* str);
void map_str(string_t* str, char map(char));

str_iter_t iter_from_cstr(const char* str, size_t len);
str_iter_t iter_from_str(const string_t* str);
char str_iter_peek(const str_iter_t* iter);
char str_iter_peek_n(const str_iter_t* iter, size_t n);
char str_iter_next(str_iter_t* iter);
void str_iter_skip(str_iter_t* iter, size_t skip);
void str_iter_skip_whitespace(str_iter_t* iter);

#endif