#pragma once

#include "rust_types.h"

typedef struct vector {
    void **contents;
    usize len;
    usize cap;

    struct {
        void (*push)(struct vector*, void*);
        void *(*pop)(struct vector*);
        void *(*get)(struct vector*, usize);
        void (*increase_size)(struct vector*);
    } callbacks;
} vector_t;

vector_t init_vector();
void push(vector_t *self, void *value);
void *pop(vector_t *self);
void *get(vector_t *self, usize index);
void increase_size(vector_t* self);