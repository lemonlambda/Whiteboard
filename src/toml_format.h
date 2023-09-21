#pragma once

#include <stdbool.h>

#include "debug.h"
#include "rust_types.h"
#include "vector.h"
#include "toml.h"

// @desc		Whiteboard package info struct
//
// @field(name)		Name of package
// @field(version)	Version of package
//
// @field(callbacks)	Container of all callback functions
// @field(callbacks.make_package)	Makes a package struct given the toml
typedef struct package {
    char *name;
    char *version;

    struct {
        void (*make_package)(struct package*, toml_table_t*);
    } callbacks;
} package_t;

// @desc	Create a new package
// @returns	Default package object
package_t init_package();
// @desc	Makes a package struct given the toml
// @arg(self)	`package_t` to fill out
// @arg(toml)	Whiteboard file toml
void make_package(package_t *self, toml_table_t *toml);

// @desc			Parsed format for entries in a whiteboard file
// @field(default_bin)		Is this the default binary option?
// @field(name)			Name of this configuration
// @field(srcdir)		C source directory
// @field(includedir)		C includes directory
// @field(targetdir)		C target directory
// @field(programincludedir)	Used for test bins to add include paths for their builds
// @field(cc)			C Compiler to use
// @field(ld)			Linker to use
// @field(ccargs)		Args to use during compilation
// @field(ldargs)		Args to use during linking
// @field(callbacks)		Callback function container
// @field(callbacks.print_bin)	DEBUG-only human-readable printer for `bin_t` objects
typedef struct bin {
    bool default_bin;
    char *name;
    char *srcdir;
    char *includedir;
    char *targetdir;
    char *programincludedir;
    char *cc;
    char *ld;
    char *ccargs;
    char *ldargs;

    struct {
        #ifdef DEBUG
        void (*print_bin)(struct bin *);
        #endif
    } callbacks;
} bin_t;

// @desc	Create a new `bin_t`
// @returns	A default `bin_t`
bin_t init_bin();
#ifdef DEBUG
// @desc	Print a human-readable form of a `bin_t`
// @arg(self)	Pointer to `bin_t` to print
void print_bin(bin_t *self);
#endif

// @desc				Complete whiteboard configuration structure
// @field(package)			`package_t` package information
// @field(bin)				Vector of `bin_t` configurations for [[bin]] entries
// @field(test)				Vector of `bin_t` configurations for [[test]] entries
// @field(callbacks)			Callback function container
// @field(callbacks.make_bin)		Function that creates a `bin_t` in `bin` or `test`, based on the name and whiteboard config
// @field(callbacks.make_config)	Function that fills out `config_t` based on the whiteboard config
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

// @desc		Function that creates a `bin_t` in a `config_t`
// @arg(self)		Config to apply change to
// @arg(toml)		Whiteboard configuration table
// @arg(bin_name)	Name of this `bin_t` configuration
void make_bin(config_t *self, toml_table_t *toml, char *bin_name);
// @desc	Function that fills out a `config_t`
// @arg(self)	Config to apply change to
// @arg(toml)	Whiteboard config table
void make_config(config_t *self, toml_table_t *toml);
// @desc	Create a new `config_t` object
// @returns	A default `config_t`
config_t init_config();
// @desc	Free resources allocated by a `bin_t`
// @arg(self)	`bin_t` to clean-up after
void free_bin(bin_t const self);
// @desc	Free resources allocated by a `config_t`
// @arg(self)	`config_t` to clean-up after
void free_config(config_t const self);
