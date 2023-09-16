#pragma once

#include "rust_types.h"
#include "vector.h"
#include "toml_format.h"

typedef struct command command_t;

typedef struct stage {
    char *stage_name;
    // Vector of command_t *
    vector_t commands;

    struct {
        void (*add_stage)(struct stage *, command_t );
        void (*run_stages)(struct stage *, package_t *project, bin_t *bin);
    } callbacks;
} stage_t;


stage_t run_stage();
stage_t build_stage(char *def);
stage_t clean_stage();

stage_t init_stage(char *name);
void add_stage(stage_t *self, command_t cmd);
void run_stages(stage_t *self, package_t *project, bin_t *bin);

void free_stage(stage_t self);

typedef struct command {
    char *name;
    char *command;
} command_t;

command_t new_command(char *name, char *cmd);
void free_command(command_t cmd);

char *replace_args(command_t *cmd, package_t *package, bin_t *bin);
usize find_size(char *cmd, package_t *project, bin_t *bin);
usize count_string(const char *str1, const char *str2);
