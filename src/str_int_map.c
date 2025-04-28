#include "headers/str_int_map.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FNV 1A 64-bit algorithm from wikipedia:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
// Adapted for use on null-terminated strings
size_t inline hash_str(slice_t key) {
    uint8_t* str = (uint8_t*)key.str;
    size_t h = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < key.len; ++i) {
        h = (h ^ str[i]) * FNV_PRIME;
    }

    return h;
}

// Just strncmp internally
int inline cmp_str(slice_t lhs, slice_t rhs) {
    if (lhs.len != rhs.len) {
        return 1;
    }
    return strncmp(lhs.str, rhs.str, lhs.len);
}

si_map_t inline new_map() {
    return (si_map_t){
        .capacity = DEFAULT_MAP_CAPACITY,
        .len = 0,
        .buckets = (si_bucket_t*)calloc(DEFAULT_MAP_CAPACITY, sizeof(si_bucket_t)),
    };
}

static inline si_map_t new_map_with_capacity(size_t capacity) {
    return (si_map_t){
        .capacity = capacity,
        .len = 0,
        .buckets = (si_bucket_t*)calloc(capacity, sizeof(si_bucket_t)),
    };
}

static size_t* get_bucket(const si_bucket_t* bucket, slice_t key) {
    if (bucket == NULL) {
        return NULL;
    }

    do {
        if (bucket->key.str == NULL) {
            printf("key is null while trying to get something\n");
            return NULL;
        }
        if (cmp_str(bucket->key, key) == 0) {
            return (size_t*)&bucket->value;
        }
    } while ((bucket = bucket->next));

    printf("Went through bucket without finding anything\n");
    return NULL;
}

size_t* get_map(const si_map_t* map, slice_t key) {
    size_t idx = hash_str(key) % map->capacity;

    return get_bucket(&map->buckets[idx], key);
}

static bool insert_bucket(si_bucket_t* bucket, slice_t key, size_t value) {
    if (bucket == NULL) {
        slice_t new = (slice_t){.str = strndup(key.str, key.len), .len = key.len};
        *bucket = (si_bucket_t){.key = new, .value = value, .prev = NULL, .next = NULL};
        return true;
    }

    do {
        if (bucket->key.str == NULL) {
            bucket->key.str = strndup(key.str, key.len);
            bucket->key.len = key.len;
            bucket->value = value;
            return true;
        }
        if (cmp_str(bucket->key, key) == 0) {
            return false;
        }
        if (bucket->next == NULL) {
            si_bucket_t* new = (si_bucket_t*)malloc(1 * sizeof(si_bucket_t));

            new->key.str = strndup(key.str, key.len);
            new->key.len = key.len;
            new->value = value;

            new->prev = bucket;
            bucket->next = new;

            return true;
        }
    } while ((bucket = bucket->next));

    // should be unreachable
    exit(1);
}

bool insert_map(si_map_t* map, slice_t key, size_t value) {
    // Grow if length > 0.75 * capacity
    if (map->len > map->capacity / 4 * 3) {
        si_map_t new = new_map_with_capacity(map->capacity * 2);

        si_bucket_t* bucket;

        for (size_t i = 0; i < map->capacity; ++i) {
            bucket = &map->buckets[i];
            do {
                if (bucket->key.str != NULL) {
                    insert_map(&new, bucket->key, bucket->value);
                }
            } while ((bucket = bucket->next));
        }

        printf("growing\n");

        free_map(map);
        *map = new;
    }

    size_t idx = hash_str(key) % map->capacity;

    if (insert_bucket(&map->buckets[idx], key, value)) {
        map->len++;
        return true;
    }
    return false;
}

static bool remove_bucket(si_bucket_t* bucket, slice_t key) {
    if (bucket == NULL) {
        return false;
    }

    do {
        if (bucket->key.str == NULL) {
            printf("key is null while trying to remove something\n");
            return false;
        }
        if (cmp_str(bucket->key, key) == 0) {
            free((void*)bucket->key.str);
            free(bucket);
            bucket = NULL;

            if (bucket->prev == NULL) {
                if (bucket->next != NULL) {
                    bucket->key = bucket->next->key;
                    bucket->value = bucket->next->value;
                    bucket->next = bucket->next->next;
                }
            } else {
                // if bucket->prev isn't NULL
                bucket->prev->next = bucket->next;

                if (bucket->next != NULL) {
                    bucket->next->prev = bucket->prev;
                }
            }

            return true;
        }
    } while ((bucket = bucket->next));

    printf("Went through bucket without finding anything\n");
    return false;
}

bool remove_map(si_map_t* map, slice_t key) {
    size_t idx = hash_str(key) % map->capacity;

    return remove_bucket(&map->buckets[idx], key);
}

void clear_map(si_map_t* map) {
    free_map(map);
    *map = new_map();
}

static void free_bucket(si_bucket_t* bucket) {
    assert(bucket != NULL);

    do {
        if (bucket->key.str != NULL) {
            free((void*)bucket->key.str);
        }
    } while ((bucket = bucket->next));
}

void free_map(si_map_t* map) {
    assert(map != NULL);

    if (map->buckets == NULL) {
        return;
    }

    for (size_t i = 0; i < map->capacity; ++i) {
        free_bucket(&map->buckets[i]);
    }

    free(map->buckets);
    map = NULL;
}

si_map_iter_t iter_from_map(const si_map_t* map) {
    assert(map != NULL);

    const si_bucket_t* end = &map->buckets[map->capacity - 1];

    return (si_map_iter_t){
        .ptr = map->buckets,
        .end = end,
        .bucket_idx = 0,
    };
}

si_kv map_iter_peek(const si_map_iter_t* iter) {
    assert(iter != NULL);

    si_bucket_t* ptr = (si_bucket_t*)iter->ptr;

    while (ptr->key.str == NULL && ptr != iter->end) {
        ptr++;
    }
    if (ptr == iter->end) {
        return (si_kv){.key = (slice_t){.len = 0, .str = NULL}, .value = 0};
    }
    for (size_t i = 0; i < iter->bucket_idx; ++i) {
        if (ptr->next != NULL) {
            ptr = ptr->next;
        } else {
            printf("BLOW THIS BUILDING UP NOW\n");
            exit(1);
        }
    }
    return (si_kv){.key = ptr->key, .value = ptr->value};
}

si_kv map_iter_next(si_map_iter_t* iter) {
    assert(iter != NULL);

    // printf("bucket idx: %lu\n", iter->bucket_idx);

    si_bucket_t* bucketptr = (si_bucket_t*)iter->ptr;

    while (bucketptr->key.str == NULL && bucketptr != iter->end) {
        bucketptr++;
    }
    if (bucketptr == iter->end) {
        return (si_kv){.key = (slice_t){.len = 0, .str = NULL}, .value = 0};
    }
    for (size_t i = 0; i < iter->bucket_idx; ++i) {
        if (bucketptr->next != NULL) {
            bucketptr = bucketptr->next;
        } else {
            printf("BLOW THIS BUILDING UP NOW\n");
            exit(1);
        }
    }
    if (bucketptr->next != NULL) {
        // printf("incr bucket idx\n");
        iter->bucket_idx++;
    } else {
        // printf("incr pointer\n");
        iter->bucket_idx = 0;
        iter->ptr++;
    }
    return (si_kv){.key = bucketptr->key, .value = bucketptr->value};
}
