#ifndef _ERROR_H
#define _ERROR_H

#include <stdarg.h>

#include "attr.h"

void __noret
panic(const char *fname, int line_num, int errnum, const char *fmt, ...);

#define PANIC(errnum, ...) \
	panic(__FILE__, __LINE__, (errnum), __VA_ARGS__)

#endif /* _ERROR_H */
