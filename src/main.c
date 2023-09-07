#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <err.h>

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
    printf("Default: %d\n", value->default_bin);

    toml_free(conf);
    return 0;
}