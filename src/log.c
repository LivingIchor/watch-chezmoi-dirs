#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "log.h"

static enum log_lvl min_log_lvl = LOG_WARN;
static bool do_file_log = false;
static FILE *log_file = NULL;

static const struct {
	const char name[8];
	const int color;
} log_lvl_map[] = {
	[LOG_DEBUG] = { "DEBUG", BLUE },
	[LOG_INFO] = { "INFO", 0 },
	[LOG_WARN] = { "WARN", YELLOW },
	[LOG_ERR] = { "ERROR", RED },
};

void
log_init(enum log_lvl _min_log_lvl, bool _do_file_log, const char *_log_path)
{
	if (!(do_file_log = _do_file_log) ||
		(min_log_lvl = _min_log_lvl) == LOG_NONE)
		return;

	char log_path[MAX_PATH_LEN + 1] = { 0 };
	char default_log_path[MAX_PATH_LEN + 1];
	snprintf(default_log_path, MAX_PATH_LEN + 1,
		"%s/.watchdata/log", getenv("CHEZMOI_SOURCE_DIR"));

	// TODO: handle str func errors
	if (_log_path != NULL && _log_path[0] != '\0') {
		strncpy(log_path, _log_path, MAX_PATH_LEN + 1);
	} else {
		strncpy(log_path, default_log_path, MAX_PATH_LEN + 1);
	}

	char *elem_head = log_path;
	for (size_t i = 0; i < MAX_PATH_LEN + 1; i++) {
		if (log_path[i] == '\0')
			break;

		if (log_path[i] == '/') {
			log_path[i] = '\0';

			// TODO: handle when already exists & other error
			mkdir(log_path, 0755);

			log_path[i] = '/';
			elem_head = &log_path[i + 1];
		}
	}

	struct stat statbuf;
	if (
		*elem_head == '\0' ||
		(stat(log_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)))
	{
		WLOG("Log file path is a dir - defaulting to %s", default_log_path);
		log_init(_min_log_lvl, _do_file_log, NULL);
		return;
	}

	size_t reattempts = 3;
	do {
		if ((log_file = fopen(log_path, "a")) != NULL) {
			return;
		}

		ELOG("Failed to open log file");
		reattempts--;
	} while (reattempts > 0);

	if (reattempts == 0) {
		ELOG("Timeout opening log file - disabling log");
		do_file_log = false;
	}
}

void
log_deinit(void)
{
	if (do_file_log)
		fclose(log_file);
}

static void
_cmd_logger(
	enum log_lvl level,
	const char *fname,
	const int line_num,
	const char *fmt,
	va_list va
)
{
	const char *name = log_lvl_map[level].name;
	const int color = log_lvl_map[level].color;

	fprintf(stderr, "\x1b[%dm[%s]: ", color, name);
	vfprintf(stderr, fmt, va);
	fprintf(stderr, " in %s at line %d\x1b[0m\n", fname, line_num);
}

static void
_file_logger(
	enum log_lvl level,
	const char *fname,
	const int line_num,
	const char *fmt,
	va_list va
)
{
	const char *name = log_lvl_map[level].name;

	fprintf(log_file, "[%s]: ", name);
	vfprintf(log_file, fmt, va);
	fprintf(log_file, " in %s at line %d\n", fname, line_num);
}

void
log_msg(
	enum log_lvl level,
	const char *fname,
	const int line_num,
	const char *fmt,
	...
)
{
	if (level < min_log_lvl)
		return;

	va_list va;
	va_list va_cpy;

	va_start(va, fmt);
	va_copy(va_cpy, va);

	_cmd_logger(level, fname, line_num, fmt, va);
	_file_logger(level, fname, line_num, fmt, va_cpy);

	va_end(va);
	va_end(va_cpy);
}
