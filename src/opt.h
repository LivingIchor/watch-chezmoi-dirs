#ifndef _OPT_H
#define _OPT_H

#include <stddef.h>

enum arg_needs {
	unrecognized_option,
	no_argument,
	optional_argument,
	required_argument
};

struct long_opt {
	const char *opt;
	const enum arg_needs need;
	const char symbol;
};

extern size_t argi;
extern const char *arg;

char
get_opt(char *argv[], const char opts_str[], const struct long_opt long_opts[]);

#endif /* _OPT_H */
