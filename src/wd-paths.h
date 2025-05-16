#ifndef _WD_TREE_H
#define _WD_TREE_H

#include <stdbool.h>

typedef struct paths *paths_t;

paths_t
paths_create(void);

void
paths_destroy(paths_t start);

void
paths_add(paths_t start, int wd, char *path, bool is_dir);

void
paths_remove(paths_t start, int wd);

const char *
paths_retrieve(paths_t start, int wd);

#endif /* _WD_TREE_H */
