#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "opt.h"
#include "error.h"

enum _opt_mode {
	no_mode,
	short_mode,
	long_mode
};

size_t argi = 1;
const char *arg = NULL;

char
get_opt(char *argv[], const char opts_str[], const struct long_opt long_opts[])
{
	if (argv[argi] == NULL)
		return 0;

	static enum arg_needs short_opts[52] = { 0 };
	static bool set_opts_str = false;
	enum arg_needs *curr_symbol = NULL;
	for (size_t i = 0; !set_opts_str && opts_str[i] != '\0'; i++) {
		switch (opts_str[i]) {
		case 'A' ... 'Z':
			curr_symbol = &short_opts[opts_str[i] - 'A'];
			if (*curr_symbol >= no_argument)
				PANIC(-1, "Malformed option string");

			(*curr_symbol)++;
			break;
		case 'a' ... 'z':
			curr_symbol = &short_opts[opts_str[i] - 'a'];
			if (*curr_symbol >= no_argument)
				PANIC(-1, "Malformed option string");

			(*curr_symbol)++;
			break;
		case ':':
			if (curr_symbol == NULL || *curr_symbol >= required_argument)
				PANIC(-1, "Malformed option string");

			(*curr_symbol)++;
			break;
		default:
			PANIC(-1, "Malformed option string");
		}
	}
	set_opts_str = true;

	static size_t optind = 0;
	static enum _opt_mode curr_mode = no_mode;
	static bool arg_captured = false;

	if (arg_captured && optind == 0) {
		if (argv[argi] == NULL)
			return 0;

		arg_captured = false;
		argi++;
	}

	const char *currarg = argv[argi];
	if (currarg == NULL)
		return 0;
	
	if (curr_mode == no_mode) {
		while (currarg[optind] != '\0') {
			if (currarg[optind] == '-') {
				if (optind > 1)
					return '?';

				curr_mode++;
				optind++;
			} else {
				break;
			}
		}
	}

	if (curr_mode == no_mode) {
		return -1;
	} else if (curr_mode == short_mode) {
		char ch = currarg[optind];

		size_t short_ind;
		switch (ch) {
		case 'A' ... 'Z':
			short_ind = ch - 'A';
			break;
		case 'a' ... 'z':
			short_ind = ch - 'a';
			break;
		default:
			fprintf(stderr, "Unrecognized option: %c\n", ch);
			return '?';
		}

		switch (short_opts[short_ind]) {
		case unrecognized_option:
			fprintf(stderr, "Unrecognized option: %c\n", ch);
			return '?';
		case no_argument:
		case optional_argument:
			break;
		case required_argument:
			if (argv[argi + 1] == NULL || *argv[argi + 1] == '-') {
				fprintf(stderr, "%c requires argument\n", ch);
				return '?';
			}

			if (arg_captured) {
				fputs("Two options can't use the same argument", stderr);
				return '?';
			}

			arg = argv[argi + 1];
			arg_captured = true;

			break;
		default:
			PANIC(-1, "Malformed option string");
		}

		if (currarg[++optind] == '\0') {
			argi++;
			optind = 0;
			curr_mode = no_mode;
		}
		
		return ch;
	} else {
		if (currarg[optind] == '\0') {
			argi++;
			return 0;
		}

		const struct long_opt *long_match = NULL;
		size_t match_len = 0;
		for (size_t i = 0; long_opts[i].opt != NULL; i++) {
			const char *name = long_opts[i].opt;
			optind = strlen(name);
			if (strncmp(&currarg[optind], name, optind) == 0 &&
				optind > match_len) {
				long_match = &long_opts[i];
				match_len = optind;
			}
		}

		if (long_match == NULL) {
			fputs("Unrecognized long option", stderr);
			return '?';
		}

		switch (long_match->need) {
		case no_argument:
			if (currarg[match_len] == '=') {
				fprintf(stderr,
					"%s takes no arguments\n", long_match->opt);
				return '?';
			}

			if (currarg[match_len] != '\0') {
				fprintf(stderr,
					"%s is not a recognized option", currarg);
				return '?';
			}

			break;
		case optional_argument:
			if (currarg[match_len] == '=')
				arg = &currarg[match_len + 1];

			if (currarg[match_len] != '\0') {
				fprintf(stderr,
					"%s is not a recognized option", currarg);
				return '?';
			}

			break;
		case required_argument:
			if (currarg[match_len] == '\0') {
				fprintf(stderr,
					"%s requires an argument\n", long_match->opt);
				return '?';
			}

			if (currarg[match_len] != '=') {
				fprintf(stderr,
					"%s is not a recognized argument", currarg);
				return '?';
			}

			arg = &currarg[match_len + 1];

			break;
		case unrecognized_option:
		default:
			PANIC(-1, "Malformed long opt struct");
		}

		argi++;
		return long_match->symbol;
	}

	PANIC(-1, "This point shouldn't be reached");
}
