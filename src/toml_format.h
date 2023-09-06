#pragma once

#include <stdbool.h>

#include "rust_types.h"
#include "vector.h"
#include "toml.h"

typedef struct package {
    char *name;
    char *version;

    struct {
        void (*make_package)(struct package *self, toml_table_t *toml);
    } callbacks;
} package_t;

void make_package(package_t *self, toml_table_t *toml);
package_t init_package();

typedef struct bin {
    bool default_bin;
    char *name;

    struct {
    } callbacks;
} bin_t;

bin_t init_bin();

typedef struct config {
    package_t package;
    // Vector of `bin_t`s
    vector_t bin;

    struct {
        void (*make_bin)(struct config *self, toml_table_t *toml);
    } callbacks;
} config_t;

void make_bin(config_t *self, toml_table_t *toml);
config_t init_config();
