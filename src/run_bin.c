#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <err.h>

#include "args_parser.h"
#include "toml_format.h"
#include "run_bin.h"
#include "color_codes.h"
#include "stages.h"

void build_bin(package_t *package, bin_t *bin, args_t *args) {
    if (args->build_mode) {
        stage_t stage = build_stage(NULL);
        stage.callbacks.run_stages(&stage, package, bin);
    }
    if (args->run_mode) {
        stage_t stage = run_stage();
        stage.callbacks.run_stages(&stage, package, bin);
    }
    if (args->clean_mode) {
        stage_t stage = clean_stage();
        stage.callbacks.run_stages(&stage, package, bin);
    }
    errx(1, "Not valid modes inputted into `build_bin`");
}

void run_whiteboard(args_t *args) {
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

        if (value == NULL && !value->default_bin) {
            errx(1, "Can't find a build by that name\n");
            break;
        } else if (value == NULL && value->default_bin) {
            errx(1, "There are no default builds\n");
            break;
        }

        if (strcmp(value->name, args->build_name) == 0 || (args->default_build && value->default_bin)) {
            build_bin(&config.package, value, args);
            break;
        }
    }
    
    toml_free(conf);
    free_config(config);
}