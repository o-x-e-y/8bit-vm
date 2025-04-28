#ifndef __OXEY_STR_INT_MAP_H
#define __OXEY_STR_INT_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "oslice.h"

#define FNV_OFFSET_BASIS 0xcbf29ce484222325ULL
#define FNV_PRIME 0x100000001b3ULL
#define DEFAULT_MAP_CAPACITY 16

typedef struct si_kv {
    slice_t key;
    size_t value;
} si_kv;

typedef struct si_bucket_t {
    slice_t key;
    size_t value;
    struct si_bucket_t* prev;
    struct si_bucket_t* next;
} si_bucket_t;

typedef struct si_map_t {
    size_t capacity;
    size_t len;
    si_bucket_t* buckets;
} si_map_t;

typedef struct si_map_iter_t {
    si_bucket_t* ptr;
    const si_bucket_t* end;
    size_t bucket_idx;
} si_map_iter_t;

// FNV 1A 64-bit algorithm from wikipedia:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
size_t hash_str(slice_t key);

// Just strcmp internally
int cmp_str(slice_t lhs, slice_t rhs);

si_map_t new_map();
// si_map_t new_map_with_capacity(size_t capacity);
inline size_t capacity_map(const si_map_t* map) { return map->capacity; }
inline size_t len_map(const si_map_t* map) { return map->len; }

size_t* get_map(const si_map_t* map, slice_t key);
bool insert_map(si_map_t* map, slice_t key, size_t value);
bool remove_map(si_map_t* map, slice_t key);
void clear_map(si_map_t* map);
void free_map(si_map_t* map);

si_map_iter_t iter_from_map(const si_map_t* str);
si_kv map_iter_peek(const si_map_iter_t* iter);
si_kv map_iter_next(si_map_iter_t* iter);

#endif
