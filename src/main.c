#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <err.h>

#include "rust_types.h"
#include "toml.h"
#include "toml_format.h"

int main() {
    FILE *fp;
    char errorBuffer[200];

    fp = fopen("whiteboard.toml", "r");
    if (!fp) {
        err(1, "Failed to open whiteboard.toml");
    }

    // Actually parse the toml now
    toml_table_t* conf = toml_parse_file(fp, errorBuffer, sizeof(errorBuffer));
    fclose(fp);

    if (!conf) {
        err(1, "Can't parse", errorBuffer);
    }

    package_t package = init_package();
    package.callbacks.make_package(&package, conf);

    toml_free(conf);
    return 0;
}