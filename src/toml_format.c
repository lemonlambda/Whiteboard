#include "toml_format.h"
#include "vector.h"
#include "platform_specific.h"
#include "debug.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define streq(str1, str2) strcmp(str1, str2) == 0

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
        errx(1, "Package doesn't exist in whiteboard.toml");
    }
  
    toml_datum_t name = toml_string_in(package, "name");
    toml_datum_t version = toml_string_in(package, "version");


    self->name = name.u.s;
    self->version = version.u.s;
}

#ifdef DEBUG
void print_bin(bin_t *self) {
    printf("struct bin {\n\tdefault_bin: %d\n\tname: %s\n\tsrcdir: %s\n\tincludedir: %s\n\ttargetdir: %s\n\tprogramincludedir: %s\n}\n", self->default_bin, self->name, self->srcdir, self->includedir, self->targetdir, self->programincludedir);
}
#endif

bin_t init_bin() {
    bin_t bin;
    bin.default_bin = false;
    bin.name = NULL;
    bin.srcdir = strdup("src");
    bin.cc = strdup("gcc");
    bin.ld = strdup("gcc");
    bin.ccargs = strdup("-O2 -Wall -Wextra"); // defaults as per issue/PR #8
    bin.ldargs = strdup("-B gcc");
    #ifdef WIN32
        bin.includedir = strdup("src\\include");
        bin.targetdir = strdup("target");
        // Used for tests
        bin.programincludedir = strdup("src\\include");
    #else
        bin.includedir = strdup("src/include");
        bin.targetdir = strdup("target");
        // Used for tests
        bin.programincludedir = strdup("src/include");
    #endif
    #ifdef DEBUG
        bin.callbacks.print_bin = &print_bin;
    #endif
    return bin;
}

void make_bin(config_t *self, toml_table_t *toml, char *bin_name) {
    #ifdef DEBUG
        printf("BIN Name: %s\n", bin_name);
    #endif
    toml_array_t *array = toml_array_in(toml, bin_name);
    // Early return because said thing doesn't exist in the toml
    if (!array)
        return;
    bool default_defined_already = false;

    for (int i = 0; ; i++) {
        toml_table_t *table = toml_table_at(array, i);
        if (!table)
            break;

        bin_t *bin = malloc(sizeof(bin_t));
    	*bin = init_bin();

        toml_datum_t name = toml_string_in(table, "name");
        if (!name.ok)
            errx(1, "There is no name for the %s", bin_name);
        toml_datum_t default_bin = toml_bool_in(table, "default");
        if (default_bin.ok)
            bin->default_bin = (bool)default_bin.u.b;

        // Dirs
        toml_datum_t srcdir = toml_string_in(table, "srcdir");
        if (srcdir.ok) {
	    free(bin->srcdir);
            bin->srcdir = srcdir.u.s;
	}
        toml_datum_t includedir = toml_string_in(table, "includedir");
        if (includedir.ok) {
	    free(bin->includedir);
            bin->includedir = includedir.u.s;
	}
        toml_datum_t targetdir = toml_string_in(table, "targetdir");
        if (targetdir.ok) {
	    free(bin->targetdir);
            bin->targetdir = targetdir.u.s;
	}
        toml_datum_t programincludedir = toml_string_in(table, "programincludedir");
        if (programincludedir.ok) {
	    free(bin->programincludedir);
            bin->programincludedir = programincludedir.u.s;
	}

        if (bin->default_bin && !default_defined_already)
            default_defined_already = true;
        else if (bin->default_bin && default_defined_already)
            errx(1, "Default bin already defined in bin: `%s`", name.u.s);

	// Compiler & Linker
	toml_datum_t cc = toml_string_in(table, "cc");
	if (cc.ok) {
		free(bin->cc);
		bin->cc = cc.u.s;
	}
	toml_datum_t ld = toml_string_in(table, "ld");
	if (ld.ok) {
		free(bin->ld);
		bin->ld = ld.u.s;
	}
	toml_datum_t ccargs = toml_string_in(table, "ccargs");
	if (ccargs.ok) {
		free(bin->ccargs);
		bin->ccargs = ccargs.u.s;
	}
	toml_datum_t ldargs = toml_string_in(table, "ldargs");
	if (ldargs.ok) {
		free(bin->ldargs);
		bin->ldargs = ldargs.u.s;
	}

        bin->name = name.u.s;
        #ifdef DEBUG
            bin->callbacks.print_bin(bin);
        #endif
        if (streq(bin_name, "bin"))
            self->bin.callbacks.push(&self->bin, bin);
        else if (streq(bin_name, "test"))
            self->test.callbacks.push(&self->test, bin);
        else
            errx(1, "Not a valid name for make_bin");
    }
}

void make_config(config_t *self, toml_table_t *toml) {
    self->package.callbacks.make_package(&self->package, toml);
    self->callbacks.make_bin(self, toml, "test");
    self->callbacks.make_bin(self, toml, "bin");
}

// Inits a blank config
config_t init_config() {
    config_t config;
    config.package = init_package();
    config.bin = init_vector();
    config.test = init_vector();
    config.callbacks.make_bin = &make_bin;
    config.callbacks.make_config = &make_config;
    return config;
}

void free_bin(bin_t const self)
{
	if (self.name != NULL)
		free(self.name);
	free(self.cc);
	free(self.ld);
	free(self.ccargs);
	free(self.ldargs);
	free(self.srcdir);
	free(self.includedir);
	free(self.targetdir);
	free(self.programincludedir);
}

void free_config(config_t const self) {
	free(self.package.name);
	free(self.package.version);
	for (usize i = 0; i < self.bin.len; ++i) {
		bin_t *pbin = self.bin.contents[i];
		free_bin(*pbin);
		free(pbin);
	}
	free_vector(self.bin);
	for (usize i = 0; i < self.test.len; ++i) {
		bin_t *pbin = self.test.contents[i];
		free_bin(*pbin);
		free(pbin);
	}
	free_vector(self.test);
}

