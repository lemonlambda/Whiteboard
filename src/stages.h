#pragma once

#include "vector.h"

typedef struct command command_t;

typedef struct stage {
    char *stage_name;
    // Vector of command_t *
    vector_t commands;

    struct {
        void (*add_stage)(struct stage *, command_t );
    } callbacks;
} stage_t;


stage_t run_stage();
stage_t build_stage(char *default);
stage_t clean_stage();

stage_t init_stage(char *name);
void add_stage(stage_t *self, command_t cmd);

void free_stage(stage_t self);

typedef struct command {
    char *name;
    char *command;
} command_t;

command_t new_command(char *name, char *cmd);
void free_command(command_t cmd);