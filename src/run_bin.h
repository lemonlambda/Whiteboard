#pragma once

#include "args_parser.h"
#include "toml_format.h"

// @desc		Runs stage based on `args_t` provided
// @arg(package)	Package config, passed to stage
// @arg(bin)		Bin config, passed to stage
void build_bin(package_t *package, bin_t *bin, args_t *args);
// @desc		Runs whiteboard, parsing toml and such based on `args`
// @arg(args)		`args_t` object to run on
void run_whiteboard(args_t *args);
