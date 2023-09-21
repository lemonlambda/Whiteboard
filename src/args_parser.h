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

// @desc	Provides a more useful string for printing, if the string is empty or NULL.
// @arg(value)	Input string
// @returns	If `value` is NULL, then "NULL".
// @returns	If `strlen(value)` is 0, then "EMPTY_STRING".
// @returns	Otherwise, `value`.
const char *null_str(const char *value);

// @desc	Creates a new `args_t`
// @returns	Default `args_t`
args_t init_args();
// @desc	Cleans up memory allocated by use of `args_t`
void free_args(args_t args);
// @desc	Produces a human-readable form of `args_t`
// @arg(args)	`args_t` object to convert
// @returns	Stringified version of `args`
char *to_string(args_t args);

// @desc	Checks if `arg` doesn't match `"--"`. If so, duplicates `arg` to `args->build_name`.
// @arg(arg)	Argument to check, should be `"--"` or a build name.
// @arg(*args)	`arg_t` object to change, if `arg` isn't the default build.
// @returns	True if default build, false otherwise.
bool check_default_build(const char *arg, args_t *args);
// @desc	Parses arguments into an `arg_t` format.
// @arg(argc)	Length of argv array
// @arg(argv)	Array of strings from command line arguments
// @returns	An `args_t` object representing the command line arguments
args_t parse_args(int argc, const char **argv);
