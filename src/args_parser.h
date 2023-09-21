#pragma once

#include <stdbool.h>

// @desc			Parsed command-line arguments
//
// @field(build_name)		Which build to run
// @field(run_args)		Command line options (--args)
// @field(default_build)	If the command will use the default build
//
// @field(run_mode)		If the command will run
// @field(build_mode)		If the command will build
// @field(clean_mode)		If the command will clean
// @field(quiet_mode)		If the command is set to be quiet
// @field(test_mode)		If the command will run a test
//
// @field(run_func)		If the command has been provided a function to run (see modes above)
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
char *to_string(args_t args);

bool check_default_build(const char *arg, args_t *args);
args_t parse_args(int argc, const char **argv);

