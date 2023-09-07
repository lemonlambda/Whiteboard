#include "toml_format.h"
#include "vector.h"

#include <err.h>
#include <stdbool.h>

// Inits a blank package
package_t init_package() {
    package_t package;
    package.name = NULL;
    package.version = NULL;
    package.callbacks.make_package = &make_package;
    return package;
}

// Makes a package struct given the toml
void make_package(package_t *self, toml_table_t *toml) {
    toml_table_t *package = toml_table_in(toml, "package");
    if (!package) {
        err(1, "Package doesn't exist in whiteboard.toml");
    }
  
    toml_datum_t name = toml_string_in(package, "name");
    toml_datum_t version = toml_string_in(package, "version");

    self->name = name.u.s;
    self->version = version.u.s;
}

bin_t init_bin() {
    bin_t bin;
    bin.default_bin = false;
    bin.name = NULL;
    bin.srcdir = "src";
    bin.includedir = "src/include";
    bin.targetdir = "target";
    return bin;
}

void make_bin(config_t *self, toml_table_t *toml) {
    toml_array_t *array = toml_array_in(toml, "bin");
    if (!array) {
        err(1, "Bins doesn't exist in whiteboard.toml");
    }
    bool default_defined_already = false;
    
    for (int i = 0; ; i++) {
        toml_table_t *table = toml_table_at(array, i);
        if (!table)
            break;
        
        bin_t bin = init_bin();
        
        toml_datum_t name = toml_string_in(table, "name");
        if (!name.ok)
            err(1, "There is no name for the bin");
        toml_datum_t default_bin = toml_bool_in(table, "default");
        if (!default_bin.ok)
            bin.default_bin = (bool)default_bin.u.b;
        
        // Dirs
        toml_datum_t srcdir = toml_string_in(table, "srcdir");
        if (srcdir.ok)
            bin.srcdir = srcdir.u.s;
        toml_datum_t includedir = toml_string_in(table, "includedir");
        if (includedir.ok)
            bin.includedir = includedir.u.s;
        toml_datum_t targetdir = toml_string_in(table, "targetdir");
        if (targetdir.ok)
            bin.targetdir = targetdir.u.s;

        if (bin.default_bin && !default_defined_already)
            default_defined_already = true;
        else if (bin.default_bin && default_defined_already)
            err(1, "Default bin already defined in bin: `%s`", name.u.s);

        bin.name = name.u.s;
        self->bin.callbacks.push(&self->bin, (void *)&bin);

    }
}

void make_config(config_t *self, toml_table_t *toml) {
    self->package.callbacks.make_package(&self->package, toml);
    self->callbacks.make_bin(self, toml);
}
// Inits a blank config
config_t init_config() {
    config_t config;
    config.package = init_package();
    config.bin = init_vector();
    config.callbacks.make_bin = &make_bin;
    config.callbacks.make_config = &make_config;
    return config;
}

