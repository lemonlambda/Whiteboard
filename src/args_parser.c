#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "rust_types.h"
#include "args_parser.h"
#include "err.h"

#define maybe_free(ptr) if (ptr != NULL) { free((void *)ptr); ptr = NULL; }

args_t init_args() {
    args_t args;
    
    args.run_mode = false;
    args.build_mode = false;
    args.clean_mode = false;
    args.test_mode = false;
    
    args.default_build = true;
    args.quiet_mode = false;
    args.run_func = false;
    args.clean_mode = false;
    args.build_name = calloc(1, sizeof(const char *));
    args.run_args = calloc(1, sizeof(char *));
    return args;
}

const char *null_str(const char *value) {
    return (value ? (strcmp(value, "") == 0 ? "EMPTY_STRING" : value): "NULL");
}

char *to_string(args_t args) {
    const char *null_build_name = null_str(args.build_name);
    const char *null_run_args = null_str(args.run_args);

    char *format = "\
struct args \n\
{\n\
	build_name: %s\n\
	run_args: %s\n\
	run_mode: %i\n\
	build_mode: %i\n\
	clean_mode: %i\n\
	test_mode: %i\n\
	default_build: %i\n\
	quiet_mode: %i\n\
	run_func: %i\n\
}";
    char displayed[strlen(format) + strlen(null_build_name) + strlen(null_run_args) + 1 + 1 + 1 + 1 + 1];
    sprintf(
        displayed, 
        format,
        null_build_name, null_run_args, args.run_mode, args.build_mode, args.clean_mode, args.test_mode, args.default_build, args.quiet_mode, args.run_func
    );
    char *final = malloc(sizeof(char) * (strlen(displayed) + 1));
    strcpy(final, displayed);
    return final;
}

void free_args(args_t args) {
    maybe_free(args.build_name);
    maybe_free(args.run_args);
}

bool check_default_build(const char *arg, args_t *args) {
    if (strcmp(arg, "--") != 0) {
	free((void*)args->build_name);
        args->build_name = strdup(arg);
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
            // I don't know if I should be putting this here but I am so let's see if it fucks up
            // later down the line
            args.default_build = false;
            got_two_dashes = check_default_build(arg, &args);
        } else if (got_two_dashes) {
            char formatted[strlen(arg) + 5];
            sprintf(formatted, "%s ", arg);
            args.run_args = realloc(args.run_args, sizeof(char) * (strlen(args.run_args) + strlen(formatted) + 1));
	        assert(args.run_args != NULL);
            strcat(args.run_args, formatted);
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
            if (strcmp(arg, "test") == 0) {
                args.test_mode = true;
                args.run_func = true;
                if (argc < 3)
                    errx(1, "You must provide the name of the test that you want to test");
            }
        }
    }

    return args;
}

