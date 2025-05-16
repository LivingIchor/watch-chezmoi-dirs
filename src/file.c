#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <openssl/evp.h>

#include "file.h"
#include "error.h"
#include "wd-paths.h"

#define MAGIC_BYTES "CZMWATCH"
#define MAGIC_LEN 8
#define MAX_PATH_LEN 4096

typedef struct {
    char magic[MAGIC_LEN];
    uint32_t num_entries;
    uint8_t hash[32];
} header_t;

typedef struct {
    bool is_dir;
    uint32_t path_len;
    char path[MAX_PATH_LEN + 1];
} entry_t;

void
_calculate_file_hash(
	entry_t *entries,
	uint32_t num_entries,
	unsigned char hash[32]
)
{
	EVP_MD_CTX *mdctx;
	unsigned int hash_len;

	// Create and initialize the context
	mdctx = EVP_MD_CTX_new();
	if (mdctx == NULL)
		PANIC(-1, "Error creating MD context");

	// Initialize with SHA-256 algorithm
	if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
		EVP_MD_CTX_free(mdctx);
		PANIC(-1, "Error initializing digest");
	}

	// Hash each entry
	for (uint32_t i = 0; i < num_entries; i++) {
		EVP_DigestUpdate(mdctx, &entries[i].is_dir, sizeof(uint8_t));
		EVP_DigestUpdate(mdctx, &entries[i].path_len, sizeof(uint32_t));
		EVP_DigestUpdate(mdctx, entries[i].path, entries[i].path_len);
	}

	// Finalize and get the digest
	if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
		PANIC(-1, "Error finalizing digest");
	}

	// Clean up
	EVP_MD_CTX_free(mdctx);
}

paths_t
file_load(int infd, const char *list_path)
{
	paths_t paths = paths_create();

	errno = 0;
	struct stat statbuf;
	if (stat(list_path, &statbuf) == -1) {
		switch (errno) {
		case ENOENT:
			return paths;
		default:
			PANIC(errno, "Error reading list paths");
		}
	}
	
	if (!S_ISREG(statbuf.st_mode)) {
		printf("%s: Is not a regular file\n", list_path);
		return NULL;
	}

	if (statbuf.st_size == 0)
		return paths;

	header_t header;

	errno = 0;
	FILE *fp;
	if ((fp = fopen(list_path, "rb")) == NULL)
		PANIC(errno, "Error opening list of paths");
	errno = 0;
	if (fread(&header, sizeof(header_t), 1, fp) < 1)
		PANIC(errno, "Error reading list of paths");

	if (strncmp(header.magic, MAGIC_BYTES, MAGIC_LEN) != 0) {
		printf("%s: Is not a watch save file\n", list_path);
		fclose(fp);
		return NULL;
	}

	entry_t entries[header.num_entries];
	for (uint32_t i = 0; i < header.num_entries; i++) {
		if (fread(&entries[i].is_dir, sizeof(bool), 1, fp) != 1) {
			printf("%s: Contains corrupted save data\n", list_path);
			fclose(fp);
			return NULL;
		}

		if (fread(&entries[i].path_len, sizeof(uint32_t), 1, fp) != 1
			|| entries[i].path_len > MAX_PATH_LEN) {
			printf("%s: Contains corrupted save data\n", list_path);
			fclose(fp);
			return NULL;
		}

		if (fread(&entries[i].path, entries[i].path_len, 1, fp) != 1) {
			printf("%s: Contains corrupted save data\n", list_path);
			fclose(fp);
			return NULL;
		}

		entries[i].path[entries[i].path_len] = '\0';
	}

	uint8_t calculated_hash[32];
	_calculate_file_hash(entries, header.num_entries, calculated_hash);

	if (memcmp(calculated_hash, header.hash, 32) != 0) {
		printf("%s: Is corrupted\n", list_path);
		fclose(fp);
		return NULL;
	}

	int wd = inotify_add_watch(infd, list_path, 0);
	paths_add(paths, wd, strdup(list_path), false);
	for (size_t i = 0; i < header.num_entries; i++) {
		wd = inotify_add_watch(infd, list_path, 0);
		paths_add(paths, wd, strdup(entries[i].path), entries[i].is_dir);
	}

	fclose(fp);
	return paths;
}

void
file_dump(paths_t paths, const char *list_path)
{
}
