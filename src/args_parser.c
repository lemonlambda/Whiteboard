#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <assert.h>

#include "rust_types.h"
#include "args_parser.h"

bool run(const char *arg, const char **pbuild_name, bool build_mode) {
    if (strcmp(arg, "--") != 0) {
        *pbuild_name = arg;
        return false;
    }
    return true;
}

// run_args must be on the heap (we use realloc)
bool parse_args(int argc, const char **argv, bool *run_mode, bool *default_build, bool *quiet_mode, const char **pbuild_name, char **prun_args) {
    bool got_two_dashes = false;
    bool run_func = false;
    bool build_mode = false;
    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--quiet") == 0) {
            *quiet_mode = true;
            continue;
        }
        
        if ((run_func || build_mode) && !got_two_dashes) {
            got_two_dashes = run(arg, pbuild_name, build_mode);
        } else if (got_two_dashes) {
            char formatted[strlen(arg) + 5];
            sprintf(formatted, "%s ", arg);
            *prun_args = realloc(*prun_args, sizeof(char) * (strlen(*prun_args) + strlen(formatted) + 1));
	    assert (*prun_args != NULL);
            strcat(*prun_args, formatted);
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

    return run_func;
}

