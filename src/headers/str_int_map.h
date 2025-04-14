#ifndef __OXEY_STR_INT_MAP_H
#define __OXEY_STR_INT_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "oslice.h"

#define FNV_OFFSET_BASIS 0xcbf29ce484222325ULL
#define FNV_PRIME 0x100000001b3ULL
#define DEFAULT_MAP_CAPACITY 16

typedef struct kv {
    slice_t key;
    size_t value;
} kv;

typedef struct bucket_t {
    slice_t key;
    size_t value;
    struct bucket_t* prev;
    struct bucket_t* next;
} bucket_t;

typedef struct map_t {
    size_t capacity;
    size_t len;
    bucket_t* buckets;
} map_t;

typedef struct map_iter_t {
    bucket_t* ptr;
    const bucket_t* end;
    size_t bucket_idx;
} map_iter_t;

// FNV 1A 64-bit algorithm from wikipedia:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
size_t hash_str(slice_t key);

// Just strcmp internally
int cmp_str(slice_t lhs, slice_t rhs);

map_t new_map();
// map_t new_map_with_capacity(size_t capacity);
size_t inline capacity_map(const map_t* map) { return map->capacity; }
size_t inline len_map(const map_t* map) { return map->len; }

size_t* get_map(const map_t* map, slice_t key);
bool insert_map(map_t* map, slice_t key, size_t value);
bool remove_map(map_t* map, slice_t key);
void clear_map(map_t* map);
void free_map(map_t* map);

map_iter_t iter_from_map(const map_t* str);
kv map_iter_peek(const map_iter_t* iter);
kv map_iter_next(map_iter_t* iter);

#endif
