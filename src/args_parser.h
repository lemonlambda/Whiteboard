#pragma once

#include <stdbool.h>

typedef struct args {
    const char *build_name;
    char *run_args;
    bool default_build;
    
    bool run_mode;
    bool build_mode;
    bool clean_mode;
    bool quiet_mode;
    bool test_mode;
    
    bool run_func;
} args_t;

const char *null_str(const char *value);

args_t init_args();
void free_args(args_t args);
char *to_string(args_t *args);

bool run(const char *arg, args_t *args);
args_t parse_args(int argc, const char **argv);

