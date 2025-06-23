#include "headers/str_int_map.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FNV 1A 64-bit algorithm from wikipedia:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
// Adapted for use on null-terminated strings
inline size_t hash_str(slice_t key) {
    uint8_t* str = (uint8_t*)key.str;
    size_t h = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < key.len; ++i) {
        h = (h ^ str[i]) * FNV_PRIME;
    }

    return h;
}

// Just strncmp internally
inline int cmp_str(slice_t lhs, slice_t rhs) {
    if (lhs.len != rhs.len) {
        return 1;
    }
    return strncmp(lhs.str, rhs.str, lhs.len);
}

inline si_map_t new_map() {
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
        return false;
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
            new->next = NULL;
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

    si_bucket_t* current = bucket;
    while (current != NULL) {
        if (current->key.str == NULL) {
            printf("key is null while trying to remove something\n");
            return false;
        }
        if (cmp_str(current->key, key) == 0) {
            if (current->prev == NULL) {
                // Head node removal
                if (current->next == NULL) {
                    // Only node in chain
                    free((void*)current->key.str);
                    current->key.str = NULL;
                    current->key.len = 0;
                    current->value = 0;
                } else {
                    // Replace head with next node
                    si_bucket_t* next_node = current->next;
                    free((void*)current->key.str);
                    current->key = next_node->key;
                    current->value = next_node->value;
                    current->next = next_node->next;
                    if (next_node->next) {
                        next_node->next->prev = current;
                    }
                    free(next_node);
                }
            } else {
                // Non-head node removal
                current->prev->next = current->next;
                if (current->next) {
                    current->next->prev = current->prev;
                }
                free((void*)current->key.str);
                free(current);
            }
            return true;
        }
        current = current->next;
    }

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

    si_bucket_t* current = bucket->next;
    while (current != NULL) {
        si_bucket_t* next = current->next;
        if (current->key.str != NULL) {
            free((void*)current->key.str);
        }
        free(current);
        current = next;
    }

    if (bucket->key.str != NULL) {
        free((void*)bucket->key.str);
        bucket->key.str = NULL;
    }
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
    map->buckets = NULL;
    map->capacity = 0;
    map->len = 0;
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
