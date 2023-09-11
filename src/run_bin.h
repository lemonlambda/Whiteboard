#pragma once

#include "args_parser.h"
#include "toml_format.h"

void build_bin(package_t *package, bin_t *bin, args_t *args);
void run_whiteboard(args_t *args);