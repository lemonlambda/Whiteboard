// err.h - Header for windows people!
#ifndef ERR_H
#define ERR_H

#include "platform_specific.h"
#include <stdarg.h>
#ifdef WIN32
	// @note Make sure to set this to argv[0] in main!
	extern char const *err_program_name;

	void vwarn(char const *const format, va_list args);
	void warn(char const *const format, ...);
	void vwarnx(char const *const format, va_list args);
	void warnx(char const *const format, ...);

	void verr(int status, char const *const format, va_list args) __attribute__((noreturn));
	void err(int status, char const *const format, ...) __attribute__((noreturn));
	void verrx(int status, char const *const format, va_list args) __attribute__((noreturn));
	void errx(int status, char const *const format, ...) __attribute__((noreturn));
#else
	#include <err.h>
#endif


#endif
