#pragma once

#include "platform_specific.h"

#ifdef WIN32
    #include <errno.h>
    #include <stdio.h>
    #include <string.h>

    #define errx(code, format, ...) fprintf(stderr, format, __VA_ARGS__); exit(code);
    #define err(format, ...) errx(format ": %s", strerror(errno));
#else
    #include <err.h>
#endif
