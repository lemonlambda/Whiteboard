#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <err.h>

#include "debug.h"
#include "args_parser.h"
#include "toml_format.h"
#include "run_bin.h"
#include "color_codes.h"
#include "stages.h"

void build_bin(package_t *package, bin_t *bin, args_t *args) {
    stage_t stage;
    if (args->build_mode) {
        stage = build_stage(NULL);
        stage.callbacks.run_stages(&stage, package, bin);
    } else if (args->run_mode) {
        stage = run_stage();
        stage.callbacks.run_stages(&stage, package, bin);
    } else if (args->clean_mode) {
        stage = clean_stage();
        stage.callbacks.run_stages(&stage, package, bin);
    } else if (args->test_mode) {
        stage = test_stage();
        stage.callbacks.run_stages(&stage, package, bin);
    } else {
        errx(1, "No valid modes inputted into `build_bin`");
	return; // skip past free_stage or this is UB
    }
    free_stage(stage);
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

    #ifdef DEBUG
        printf("\n%d\n\n", args->test_mode);
    #endif

    if (args->test_mode) {
        if (config.test.len == 0)
            errx(1, "There are no tests in the toml\n");
    } else {
        if (config.bin.len == 0)
            errx(1, "There are no bins in the toml\n");
    }
        
    
    for (int i = 0; ; i++) {
        bin_t *value;
        if (args->test_mode)
            value = (bin_t *)config.test.callbacks.get(&config.test, i);
        else
            value = (bin_t *)config.bin.callbacks.get(&config.bin, i);

        #ifdef DEBUG
            value->callbacks.print_bin(value);
        #endif

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
