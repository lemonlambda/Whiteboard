#pragma once

#include "rust_types.h"

// @desc			Generic vector type
//
// @field(contents)		List of pointers to generic data elements
// @field(len)			Number of elements stored
// @field(cap)			Current capacity (measured in sizeof(void*))
//
// @field(push)			Function called when pushing a new value to the top (end)
// @field(pop)			Function called when popping a value from the top (end)
// @field(get)			Function called when getting a value from an arbitrary index of the vector
// @field(increase_size)	Function called to force a `cap` increase and realloc.
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

// @desc	Create a new `vector_t`
// @returns	Default `vector_t`
vector_t init_vector();
// @desc	Push a `value` to the end of a `vector_t`
// @arg(self)	Vector to affect
// @arg(value)	Pointer to data to push
void push(vector_t *self, void *value);
// @desc	Pop a value from the end of a `vector_t`
// @arg(self)	Vector to affect
// @returns	Value that was stored at the end of the vector.
void *pop(vector_t *self);
// @desc	Get a value from an `index` of a `vector_t`
// @arg(self)	Vector to read
// @arg(index)	Where in the vector to read from
// @returns	Value that was stored at the index
void *get(vector_t *self, usize index);
// @desc	Force a `vector_t.cap` increase and realloc.
// @arg(self)	Vector to affect
void increase_size(vector_t* self);
// @desc	Free the allocations of a `vector_t`
// @arg(self)	Vector whose `contents` to free
void free_vector(vector_t const self);
