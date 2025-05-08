#include "log.h"

int
main(int argc, char *argv[])
{
	log_init(LOG_WARN, true, "./test-log");

	DLOG("debug");
	ILOG("info");
	WLOG("warning");
	ELOG("error");

	log_deinit();

	return 0;
}
