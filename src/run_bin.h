#pragma once

#include "toml_format.h"

void build_bin(package_t *package, bin_t *bin, bool run, char *run_args, bool quiet_mode);
