#pragma once

#include <stdbool.h>

bool run(const char *arg, const char *build_name, bool build_mode);
bool parse_args(int argc, const char **argv, bool *run_mode, bool *default_build, bool *quiet_mode, const char *build_name, char *run_args);

