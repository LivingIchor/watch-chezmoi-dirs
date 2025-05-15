#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "attr.h"

void __noret
panic(const char *fname, int line_num, int errnum, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	fputs("\x1b[1;31m", stderr);

	fputs("[PANIC!]: ", stderr);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, " in %s at line %d: %s",
		fname, line_num, strerror(errnum));

	fputs("\x1b[0m\n", stderr);

	va_end(va);

	exit(1);
}
