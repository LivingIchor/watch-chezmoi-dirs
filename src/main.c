#include <stdio.h>
#include <string.h>

#include "log.h"
#include "opt.h"

static void
print_usage(const char *prog_name)
{
	static const char options[] =
	"\nOptions:\n"
	"  -l,--log-level={debug|info|warn|error|none}	sets the min level logged\n"
	"  -L,--log-file=PATH				enables file logging and sets log path\n"
	"  -h,--help					print this help screen\n";

	printf("Usage: %s [OPTIONS...] [list path]\n", prog_name);
	puts(options);
}

int
main(int argc, char *argv[])
{
	char list_path[MAX_PATH_LEN + 1] = { 0 };
	snprintf(list_path, MAX_PATH_LEN + 1,
		"%s/.watchdata/watches", ("CHEZMOI_SOURCE_DIR"));

	enum log_lvl min_log_lvl = LOG_WARN;
	bool do_file_log = false;
	const char *log_path = NULL;
	

	struct long_opt long_opts[] = {
		{ "log-level", required_argument, 'l' },
		{ "log-file", required_argument, 'L' },
		{ "help", no_argument, 'h' },
		{ NULL, unrecognized_option, 0 }
	};

	const char *interum = NULL;
	while (true) {
		char opt = get_opt(argv, "l::L::h", long_opts);

		if (opt == 0)
			break;

		if (opt == -1) {
			if (interum != NULL) {
				printf("%s only takes one list path\n", argv[0]);
				return 1;
			}
			interum = argv[argi];

			argi++;
			continue;
		}

		switch (opt) {
		case 'l':
			if (strcmp(arg, "debug") == 0) {
				min_log_lvl = LOG_DEBUG;
			} else if (strcmp(arg, "info") == 0) {
				min_log_lvl = LOG_INFO;
			} else if (strcmp(arg, "warn") == 0) {
				min_log_lvl = LOG_WARN;
			} else if (strcmp(arg, "error") == 0) {
				min_log_lvl = LOG_ERR;
			} else if (strcmp(arg, "none") == 0) {
				min_log_lvl = LOG_NONE;
			} else {
				puts("Unrecognized log level: defaulting to 'warn'");
			}

			break;
		case 'L':
			do_file_log = true;
			log_path = arg;
			break;
		case 'h':
			print_usage(argv[0]);
			break;
		case '?':
			return 1;
		}
	}

	argc -= argi;
	argv += argi;

	if (interum) {
		if (argc) {
			printf("%s does not accept multiple list paths", argv[0]);
			return 1;
		}

		strncpy(list_path, interum, MAX_PATH_LEN);
	} else {
		if (argc == 1) {
			strncpy(list_path, argv[0], MAX_PATH_LEN);
		} else if (argc > 1) {
			printf("%s does not accept multiple list paths", argv[0]);
			return 1;
		}
	}

	printf("min_log_lvl: %d\n", min_log_lvl);
	printf("do_file_log: %s\n", do_file_log ? "true" : "false");
	printf("log_path: %s\n", log_path);
	printf("list_path: %s\n", list_path);
	log_init(min_log_lvl, do_file_log, log_path);

	DLOG("This is a debug log");
	ILOG("This is a info log");
	WLOG("This is a warning log");
	ELOG("This is a error log");

	log_deinit();

	return 0;
}
