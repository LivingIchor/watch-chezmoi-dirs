#ifndef _WATCH_MANAGEMENT_H
#define _WATCH_MANAGEMENT_H

#include "wd-paths.h"

struct watch_info {
	int infd;
	paths_t wd_paths;
};

void *
manage_watches(void *arg);

#endif /* _WATCH_MANAGEMENT_H */
