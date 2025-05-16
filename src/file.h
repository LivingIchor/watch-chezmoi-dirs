#ifndef _FILE_H
#define _FILE_H

#include "wd-paths.h"

paths_t
file_load(int infd, const char *list_path);

void
file_dump(paths_t paths, const char *list_path);

#endif /* _FILE_H */
