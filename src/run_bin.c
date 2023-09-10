#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <err.h>

#include "args_parser.h"
#include "toml_format.h"
#include "run_bin.h"
#include "color_codes.h"

void build_bin(package_t *package, bin_t *bin, args_t args) {
    // Make the required target dirs
    char target_dir_path[strlen(bin->targetdir) + strlen(package->name) + 2];
    sprintf(target_dir_path, "%s/%s", bin->targetdir, package->name);
    char *mkdir_f = "mkdir %s && mkdir %s/obj && mkdir %s/bin";
    char *make_dirs_command = malloc(sizeof(char) * (strlen(mkdir_f) + strlen(target_dir_path) * 2 + 2));
    sprintf(make_dirs_command, mkdir_f, target_dir_path, target_dir_path);
    if (!args.quiet_mode)
        printf("%sMaking Dirs:%s %s\n", BHMAG, CRESET, make_dirs_command);
    if (system(make_dirs_command) == -1)
        errx(1, "Recieved Error in `Making Dirs` stage");

    // Make the objs
    char *obj_f = "gcc -O2 -c $(find %s -name \"*.c\") -I %s";
    char compile_objs_command[strlen(obj_f) + strlen(bin->includedir) + strlen(bin->srcdir) + 1];
    assert(compile_objs_command != NULL);
    sprintf(compile_objs_command, obj_f, bin->srcdir, bin->includedir);
    if (!args.quiet_mode)
        printf("%sCompiling:%s %s\n", BHMAG, CRESET, compile_objs_command);
    if (system(compile_objs_command) == -1)
        errx(1, "Recieved Error in `Compiling` stage");

    // Move all the objs
    char *move_f = "mv *.o %s/obj";
    char move_objs_command[strlen(move_f) + strlen(target_dir_path) + 1];
    sprintf(move_objs_command, move_f, target_dir_path);
    if (!args.quiet_mode)
        printf("%sMoving:%s %s\n", BHMAG, CRESET, move_objs_command);
    if (system(move_objs_command) == -1)
        errx(1, "Recieved Error in `Moving` stage");

    // Link the objs
    char *link_f = "gcc -B gcc $(find %s/obj -name \"*.o\") -o %s/bin/%s-%s";
    char link_objs_command[strlen(link_f) + strlen(target_dir_path) * 2 + strlen(package->name) + strlen(package->version) + 1];
    sprintf(link_objs_command, link_f, target_dir_path, target_dir_path, bin->name, package->version);
    if (!args.quiet_mode)
        printf("%sLinking:%s %s\n", BHMAG, CRESET, link_objs_command);
    if (system(link_objs_command) == -1)
        errx(1, "Recieved Error in `Linking` stage");

    if (args.run_func) {
        char *run_f = "./%s/bin/%s-%s %s";
        char run_bin_command[strlen(run_f) + strlen(target_dir_path) + strlen(package->name) + strlen(package->version) + strlen(args.run_args) + 1];
        sprintf(run_bin_command, run_f, target_dir_path, bin->name, package->version, args.run_args);
        if (!args.quiet_mode)
            printf("%sRunning:%s %s\n", BHMAG, CRESET, run_bin_command);
        if (system(run_bin_command) == -1)
            errx(1, "Recieved Error in `Linking` stage");
    }
}

void run_whiteboard(args_t args) {
    FILE *fp;
    char errorBuffer[200];

    fp = fopen("whiteboard.toml", "r");
    if (!fp) {
        err(1, "Failed to open whiteboard.toml");
    }

    // Actually parse the toml now
    toml_table_t *conf = toml_parse_file(fp, errorBuffer, sizeof(errorBuffer));
    fclose(fp);

    if (!conf) {
        errx(1, "Can't parse: %s", errorBuffer);
    }

    config_t config = init_config();
    config.callbacks.make_config(&config, conf);
    for (int i = 0; ; i++) {
        bin_t *value = (bin_t *)config.bin.callbacks.get(&config.bin, i);

        if (value == NULL) {
            printf("Can't find a build by that name\n");
            break;
        }

        if (strcmp(value->name, args.build_name) == 0) {
            build_bin(&config.package, value, args);
            break;
        }

        if (args.default_build) {
            build_bin(&config.package, value, args);
            break;
        }
    }
    
    toml_free(conf);
    free_config(config);
}