#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <err.h>

#include "run_bin.h"
#include "rust_types.h"
#include "toml.h"
#include "toml_format.h"

i32 main() {
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
        err(1, "Can't parse", errorBuffer);
    }

    config_t config = init_config();
    config.callbacks.make_config(&config, conf);
    bin_t *value = (bin_t *)config.bin.callbacks.get(&config.bin, 0);
    printf("Name: %s\n", value->name);
    printf("Src Dir: %s\n", value->srcdir);
    printf("Include Dir: %s\n", value->includedir);
    printf("Target Dir: %s\n", value->targetdir);
    printf("Default: %d\n", value->default_bin);

    run_bin(config.package.name, value);

    toml_free(conf);
    return 0;
}