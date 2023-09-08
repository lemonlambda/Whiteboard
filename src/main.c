#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <err.h>
#include <assert.h>

#include "run_bin.h"
#include "rust_types.h"
#include "toml.h"
#include "toml_format.h"
#include "args_parser.h"

int main(int argc, const char **argv) {
    assert(argv != NULL);

    bool run_mode = false;
    bool default_build = true;
    bool quiet_mode = false;
    const char *build_name = calloc(1, sizeof(char));
    char *run_args = calloc(1, sizeof(char));

    bool ran = parse_args(argc, argv, &run_mode, &default_build, &quiet_mode, build_name, run_args);
    if (!ran)
        errx(1, "You need to provide a sub-command of: `run` or `build`");

    assert(run_args != NULL);
    assert(build_name != NULL);

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

        if (strcmp(value->name, build_name) == 0) {
            build_bin(&config.package, value, run_mode, run_args, quiet_mode);
            break;
        }

        if (default_build) {
            build_bin(&config.package, value, run_mode, run_args, quiet_mode);
            break;
        }
    }

    free(run_args);
    toml_free(conf);
    free_config(config);
    return 0;
}
