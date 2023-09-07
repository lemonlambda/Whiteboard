#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <err.h>

#include "toml_format.h"
#include "run_bin.h"
#include "color_codes.h"

void build_bin(package_t *package, bin_t *bin, bool run, char *run_args) {
    // Make the required target dirs
    char *target_dir_path = malloc(sizeof(char) * (strlen(bin->targetdir) + strlen(package->name) + 2));
    assert (target_dir_path != NULL);
    sprintf(target_dir_path, "%s/%s", bin->targetdir, package->name);
    char *mkdir_f = "mkdir -p %s/obj %s/bin";
    char *make_dirs_command = malloc(sizeof(char) * (strlen(mkdir_f) + strlen(target_dir_path) * 2 + 2));
    sprintf(make_dirs_command, mkdir_f, target_dir_path, target_dir_path);
    printf("%sMaking Dirs:%s %s\n", BHMAG, CRESET, make_dirs_command);
    if (system(make_dirs_command) == -1)
        errx(1, "Recieved Error in `Making Dirs` stage");
    free(make_dirs_command);

    // Make the objs
    char *obj_f = "gcc -O2 -c $(find %s -name \"*.c\") -I $(find %s -type d | grep \"*.h\")";
    char *compile_objs_command = malloc(sizeof(char) * (strlen(obj_f) + strlen(bin->includedir) + strlen(bin->srcdir) + 1));
    assert(compile_objs_command != NULL);
    sprintf(compile_objs_command, obj_f, bin->srcdir, bin->includedir);
    printf("%sCompiling:%s %s\n", BHMAG, CRESET, compile_objs_command);
    if (system(compile_objs_command) == -1)
        errx(1, "Recieved Error in `Compiling` stage");
    free(compile_objs_command);

    // Move all the objs
    char *move_f = "mv *.o %s/obj";
    char *move_objs_command = malloc(sizeof(char) * (strlen(move_f) + strlen(target_dir_path) + 1));
    sprintf(move_objs_command, move_f, target_dir_path);
    printf("%sMoving:%s %s\n", BHMAG, CRESET, move_objs_command);
    if (system(move_objs_command) == -1)
        errx(1, "Recieved Error in `Moving` stage");
    free(move_objs_command);

    // Link the objs
    char *link_f = "gcc -B gcc $(find %s/obj -name \"*.o\") -o %s/bin/%s-%s";
    char *link_objs_command = malloc(sizeof(char) * (strlen(link_f) + strlen(target_dir_path) * 2 + strlen(package->name) + strlen(package->version) + 1));
    sprintf(link_objs_command, link_f, target_dir_path, target_dir_path, bin->name, package->version);
    printf("%sLinking:%s %s\n", BHMAG, CRESET, link_objs_command);
    if (system(link_objs_command) == -1)
        errx(1, "Recieved Error in `Linking` stage");
    free(link_objs_command);

    if (run) {
        char *run_f = "./%s/bin/%s-%s %s";
        char *run_bin_command = malloc(sizeof(char) * (strlen(run_f) + strlen(target_dir_path) + strlen(package->name) + strlen(package->version) + strlen(run_args) + 1));
        sprintf(run_bin_command, run_f, target_dir_path, bin->name, package->version, run_args);
        printf("%sRunning:%s %s\n", BHMAG, CRESET, run_bin_command);
        if (system(run_bin_command) == -1)
            errx(1, "Recieved Error in `Linking` stage");
        free(run_bin_command);
    }

    free(target_dir_path);
}
