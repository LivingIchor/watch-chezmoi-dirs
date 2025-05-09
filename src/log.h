#ifndef _LOG_H
#define _LOG_H

#include <stdbool.h>

#define MAX_PATH_LEN 4096

#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34

enum log_lvl {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERR,
	LOG_NONE,
};

void
log_init(enum log_lvl _min_log_lvl, bool _do_file_log, char *_log_path);

void
log_deinit(void);

void
log_msg(
	enum log_lvl level,
	const char *fname,
	const int line_num,
	const char *fmt,
	...
);

#define DLOG(...) \
	log_msg(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ILOG(...) \
	log_msg(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define WLOG(...) \
	log_msg(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ELOG(...) \
	log_msg(LOG_ERR, __FILE__, __LINE__, __VA_ARGS__)

#endif /* _LOG_H */
