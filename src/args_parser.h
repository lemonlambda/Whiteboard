#pragma once

#include <stdbool.h>

bool run(char *arg, char *build_name, bool build_mode);
void parse_args(int argc, char **argv, bool *run_mode, bool *default_build, char *build_name, char *run_args);

