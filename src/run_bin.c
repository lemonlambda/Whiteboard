#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "toml_format.h"
#include "run_bin.h"

void run_bin(char *package_name, bin_t *bin) {
    // Make the required target dirs
    char *target_dir_path = malloc(sizeof(char) * (strlen(bin->targetdir) + strlen(package_name) + 2));
    sprintf(target_dir_path, "%s/%s", bin->targetdir, package_name);
    char *mkdir_f = "mkdir -p %s/obj %s/bin";
    char *make_dirs_command = malloc(sizeof(char) * (strlen(mkdir_f) + strlen(target_dir_path) * 2 + 2));
    sprintf(make_dirs_command, mkdir_f, target_dir_path, target_dir_path);
    system(make_dirs_command);
    free(make_dirs_command);

    // Make the objs
    char *obj_f = "gcc -O2 -c $(find %s -name \"*.c\") -I $(find %s -type d | grep \"*.h\")";
    char *compile_objs_command = malloc(sizeof(char) * (strlen(obj_f) + strlen(bin->includedir) + strlen(bin->srcdir) + 1));
    assert(compile_objs_command != NULL);
    sprintf(compile_objs_command, obj_f, bin->srcdir, bin->includedir);
    printf("%s\n", compile_objs_command);
    // system(compile_objs_command);
    free(compile_objs_command);
}
