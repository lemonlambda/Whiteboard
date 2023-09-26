#include "win_safe_err.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
char const *err_program_name;
#define VA_WRAP1(func, arg0) \
	va_list args; \
	va_start(args, arg0); \
	func(arg0, args); \
	va_end(args);
#define VA_WRAP2(func, arg0, arg1) \
	va_list args; \
	va_start(args, arg1); \
	func(arg0, arg1, args); \
	va_end(args);

void vwarn(char const *const format, va_list args)
{
	fprintf(stderr, "%s: ", err_program_name);
	vfprintf(stderr, format, args);
	fprintf(stderr, "%s\r\n", strerror(errno));
}
void warn(char const *const format, ...)
{
	VA_WRAP1(vwarn, format);
}
void vwarnx(char const *const format, va_list args)
{
	fprintf(stderr, "%s: ", err_program_name);
	vfprintf(stderr, format, args);
	putc('\r', stderr);
	putc('\n', stderr);
}
void warnx(char const *const format, ...)
{
	VA_WRAP1(vwarnx, format);
}

void verr(int const status, char const *const format, va_list args)
{
	vwarn(format, args);
	exit(status);
}
void err(int const status, char const *const format, ...)
{
	VA_WRAP2(verr, status, format);
}
void verrx(int const status, char const *const format, va_list args)
{
	vwarnx(format, args);
	exit(status);
}
#endif
