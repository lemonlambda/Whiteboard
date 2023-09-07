#include "vector.h"
#include <assert.h>

vector_t init_vector() {
    vector_t vector;
    vector.len = 0;
    vector.cap = 8;
    vector.contents = malloc(sizeof(void *) * vector.cap);
    assert (vector.contents != NULL);
    vector.callbacks.push = &push;
    vector.callbacks.pop = &pop;
    vector.callbacks.get = &get;
    vector.callbacks.increase_size = &increase_size;
    return vector;
}

void push(vector_t *self, void *value) {
    if (self->len >= self->cap) {
        self->callbacks.increase_size(self);
    }

    self->contents[self->len++] = value;
}

void *pop(vector_t *self) {
    return self->contents[self->len--];
}

void *get(vector_t *self, usize index) {
    assert (index < self->len);
    return self->contents[index];
}

void increase_size(vector_t *self) {
    self->cap *= 2;
    self->contents = realloc(self->contents, sizeof(void *) * self->cap);
    assert (self->contents != NULL);
}

void free_vector(vector_t const self)
{
	free(self.contents);
}
