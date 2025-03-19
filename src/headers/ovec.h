#ifndef __OXEY_VEC_H
#define __OXEY_VEC_H

#include <stdbool.h>
#include <stddef.h>

#define CAST(operation, type) (*(type*)(operation))

typedef struct {
    size_t len;
    size_t capacity;
    const size_t elem_size;

    void* ptr;
} vec_t;

typedef struct {
    size_t elem_size;

    void* ptr;
    void* end;
} vec_iter_t;

vec_t new_vec(size_t capacity, size_t elem_size);
void free_vec(vec_t* vec, void elem_destructor(void*));

size_t len_vec(vec_t* vec);
size_t capacity_vec(vec_t* vec);
size_t byte_capacity_vec(vec_t* vec);
size_t elem_size_vec(vec_t* vec);

void* get_vec(vec_t* vec, size_t index);
void* first_vec(vec_t* vec);
void* last_vec(vec_t* vec);

vec_t clone_vec(vec_t* vec);
bool eq_vec(vec_t* lhs, vec_t* rhs);

void push_vec(vec_t* vec, void* elem);
void set_vec(vec_t* vec, void* elem, size_t index);
void* pop_vec(vec_t* vec);

void insert_vec(vec_t* vec, void* elem, size_t index);
void remove_vec(vec_t* vec, size_t index);
void append_vec(vec_t* vec, vec_t* app);

void reserve_vec(vec_t* vec, size_t elements);
void resize_vec(vec_t* vec, size_t len);
void clear_vec(vec_t* vec);

void print_vec(vec_t* vec, void print_elem(void*));
void debug_vec(vec_t* vec);

vec_iter_t iter_from_vec(vec_t* vec);
void* iter_peek(vec_iter_t* iter);
void* iter_next(vec_iter_t* iter);

void map_vec(vec_t* vec, void map(void*));

#endif