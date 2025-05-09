#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "log.h"
#include "opt.h"

int
main(int argc, char *argv[])
{
	struct long_opt long_opts[] = {
		{ "testone", no_argument, 'o' },
		{ "testtwo", optional_argument, 'w' },
		{ "testthree", required_argument, 'h' },
		{ NULL, unrecognized_option, 0 }
	};

	while (true) {
		char opt = get_opt(argv, "ow:h::", long_opts);

		if (opt == 0)
			break;

		if (opt == -1) {
			printf("Found interum non-arg: %s\n", argv[argi]);
			argi++;
		}

		switch (opt) {
		case 'o':
			puts("found the testone option");
			break;
		case 'w':
			puts("found the testtwo option");
			break;
		case 'h':
			puts("found the testthree option");
			break;
		case '?':
			PANIC(-1, "Error received while parsing options");
		}
	}

	argc -= argi;
	argv += argi;

	puts("The rest of the command line was:");
	for (size_t i = 0; i < (size_t)argc; i++) {
		printf("%s ", argv[i]);
	}
	puts("\n");

	log_init(LOG_WARN, true, "./test-log");

	DLOG("debug");
	ILOG("info");
	WLOG("warning");
	ELOG("error");

	log_deinit();

	return 0;
}
