#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

#include "rust_types.h"
#include "args_parser.h"

bool run(const char *arg, const char *build_name, bool build_mode) {
    if (strcmp(arg, "--") != 0) {
        build_name = arg;
        return false;
    }
    return true;
}

void parse_args(int argc, const char **argv, bool *run_mode, bool *default_build, bool *quiet_mode, const char *build_name, char *run_args) {
    bool got_two_dashes = false;
    bool run_func = false;
    bool build_mode = false;
    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--quiet") == 0) {
            *quiet_mode = true;
        }
        
        if ((run_func || build_mode) && !got_two_dashes) {
            got_two_dashes = run(arg, build_name, build_mode);
        } else if (got_two_dashes) {
            char *formatted = malloc(sizeof(char) * (strlen(arg) + 5));
            sprintf(formatted, "%s ", arg);
            run_args = (char *)realloc(run_args, sizeof(char) * (strlen(run_args) + strlen(formatted) + 1));
            strcat(run_args, formatted);
            free(formatted);
        } else {

            if (strcmp(arg, "run") == 0) {
                *run_mode = true;
                run_func = true;
            }
            if (strcmp(arg, "build") == 0) {
                build_mode = true;
                run_func = true;
            }
        }
    }
}

