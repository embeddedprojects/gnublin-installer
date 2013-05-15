#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <archive.h>
#include <archive_entry.h>

#define PATH_MAX 256

int copy_data(struct archive* ar, struct archive* aw);
int extract_archive(const char* filename, const char* to_path);

#endif

