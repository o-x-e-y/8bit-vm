#ifndef __OXEY_UTIL_H
#define __OXEY_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include "ostring.h"
#include "oslice.h"

#define VOID_PTR_ADD(ptr, offset) ((void*)((char*)(ptr) + (offset)))

static inline size_t min(size_t a, size_t b) { return a < b ? a : b; }

static inline size_t max(size_t a, size_t b) { return a > b ? a : b; }

string_t read_file_to_str(const char* path);
void write_file(const char* path, slice_t content);

#endif