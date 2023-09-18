#pragma once

#include <stdbool.h>

#include "debug.h"
#include "rust_types.h"
#include "vector.h"
#include "toml.h"

typedef struct package {
    char *name;
    char *version;

    struct {
        void (*make_package)(struct package*, toml_table_t*);
    } callbacks;
} package_t;

void make_package(package_t *self, toml_table_t *toml);
package_t init_package();

typedef struct bin {
    bool default_bin;
    char *name;
    char *srcdir;
    char *includedir;
    char *targetdir;
    char *programincludedir;

    struct {
        #ifdef DEBUG
        void (*print_bin)(struct bin *);
        #endif
    } callbacks;
} bin_t;

bin_t init_bin();
#ifdef DEBUG
void print_bin(bin_t *self);
#endif

typedef struct config {
    package_t package;
    // Vector of `bin_t`s
    vector_t bin;
    // Vector of `bin_t`s
    vector_t test;

    struct {
        void (*make_bin)(struct config*, toml_table_t*, char *);
        void (*make_config)(struct config*, toml_table_t*);
    } callbacks;
} config_t;

void make_bin(config_t *self, toml_table_t *toml, char *bin_name);
void make_config(config_t *self, toml_table_t *toml);
config_t init_config();
void free_config(config_t const self);