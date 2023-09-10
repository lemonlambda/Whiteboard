#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

#include "rust_types.h"
#include "args_parser.h"


args_t init_args() {
    args_t args;
    args.run_mode = false;
    args.default_build = true;
    args.quiet_mode = false;
    args.run_func = false;
    const char *t_build_name = calloc(1, sizeof(char));
    char *t_run_args = calloc(1, sizeof(char));
    args.build_name = &t_build_name;
    args.run_args = &t_run_args;
    return args;
}

const char *null_str(const char **value) {
    return (value ? (*value ?: "NULL") : "NULL");
}

char *to_string(args_t *args) {
    char *format = "struct args \n{\n\tbuild_name: %s\n\trun_args: %s\n\trun_mode: %i\n\tdefault_build: %i\n\tquiet_mode: %i\n\trun_func: %i\n}";
    const char **const_run_args = (const char**)args->run_args;
    char displayed[strlen(format) + strlen(null_str(args->build_name)) + strlen(null_str(const_run_args)) + 1 + 1 + 1 + 1 + 1];
    sprintf(
        displayed, 
        format,
        null_str(args->build_name), null_str(const_run_args), args->run_mode, args->default_build, args->quiet_mode, args->run_func
    );
    char *final = malloc(sizeof(char) * (strlen(displayed) + 1));
    strcpy(final, displayed);
    return final;
}

void free_args(args_t args) {
    free((void *)args.build_name);
    free((void *)args.run_args);
}

bool run(const char *arg, args_t *args) {
    if (strcmp(arg, "--") != 0) {
        *args->build_name = arg;
        return false;
    }
    return true;
}

// run_args must be on the heap (we use realloc)
args_t parse_args(int argc, const char **argv) {
    args_t args = init_args();
    
    bool got_two_dashes = false;
    bool build_mode = false;
    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--quiet") == 0) {
            args.quiet_mode = true;
            continue;
        }
        
        if (args.clean_mode) {
            continue;
        }
        
        if ((args.run_func || build_mode) && !got_two_dashes) {
            got_two_dashes = run(arg, &args);
        } else if (got_two_dashes) {
            char formatted[strlen(arg) + 5];
            sprintf(formatted, "%s ", arg);
            *args.run_args = realloc(*args.run_args, sizeof(char) * (strlen(*args.run_args) + strlen(formatted) + 1));
	        assert (args.run_args != NULL);
            strcat(*args.run_args, formatted);
        } else {
            if (strcmp(arg, "run") == 0) {
                args.run_mode = true;
                args.run_func = true;
            }
            if (strcmp(arg, "build") == 0) {
                args.build_mode = true;
                args.run_func = true;
            }
            if (strcmp(arg, "clean") == 0) {
                args.clean_mode = true;
                args.run_func = true;
            }
        }
    }
}

