#pragma once

#include "vector.h"

typedef struct stage {
    char *stage_name;
    // Vector of char *
    vector_t commands;

    struct {
        void (*add_stage)(struct stage *, char *);
    } callbacks;
} stage_t;

stage_t run_stage();
stage_t build_stage();
stage_t clean_stage();

stage_t init_stage(char *name);
void add_stage(stage_t *self, char *cmd);

void free_stage(stage_t self);