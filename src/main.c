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

i32 main(int argc, const char **argv) {
    assert(argv != NULL);

    bool runMode = false;
    bool defaultBuild = true;
    const char *buildName;
    
    if (argc < 2)
        errx(1, "Whiteboard requires a sub-command of: `build`, or `run`");
    if (argc > 2)
        defaultBuild = false;

    if (strcmp(argv[1], "run"))
        runMode = true;

    if (!defaultBuild)
        buildName = argv[2];

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

        if (strcmp(value->name, buildName) == false) {
            build_bin(&config.package, value, runMode);
            break;
        }

        if (defaultBuild) {
            build_bin(&config.package, value, runMode);
            break;
        }
    }

    toml_free(conf);
    free_config(config);
    return 0;
}
