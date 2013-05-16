/*  archive.c
 *
 *  Author:    Michael Schwarz
 *  Copyright (C) 2011 Michael Schwarz
 *
 * Functions to handle (extract) archives
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "archive.h"
#include <stdlib.h>

int extract_archive(const char* filename, const char* to_path) {

      char command[1000];
    
    sprintf(command, "tar -xzvf %s -C %s",filename,to_path);
    system(command);
  
  
  /*
  
  struct archive* a;
  struct archive* ext;
  struct archive_entry* entry;
  int r;
  int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;

  a = archive_read_new();
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);

  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);

  if(filename != NULL && strcmp(filename, "-") == 0) {
    filename = NULL;
  }
  if((r = archive_read_open_file(a, filename, 10240))) {
    printf("archive_read_open_file(): %s\n", archive_error_string(a));
    return r;
  }
  for(;;) {
    r = archive_read_next_header(a, &entry);
    if(r == ARCHIVE_EOF) {
      break;
    }
    if(r != ARCHIVE_OK) {
      printf("archive_read_next_header(): %s\n", archive_error_string(a));
      return 0;
    }

    // rewrite pathname
    const char* path = archive_entry_pathname(entry);
    char new_path[PATH_MAX + 1];
    sprintf(new_path, "%s/%s", to_path, path + (strncmp(path, "rootfs/", 7) == 0 ? 7 : 0));
    archive_entry_set_pathname(entry, new_path);

    r = archive_write_header(ext, entry);
    if(r != ARCHIVE_OK) {
      printf("archive_write_header(): %s\n", archive_error_string(ext));
    } else {
      copy_data(a, ext);
      if(r != ARCHIVE_OK) {
        printf("archive_write_finish_entry(): %s\n", archive_error_string(ext));
        return 0;
      }

    }
    r = archive_write_finish_entry(ext);

  }
  archive_read_close(a);
  archive_read_finish(a);
  archive_write_close(ext);
  archive_write_finish(ext);
  */
  return 1;
}


int copy_data(struct archive* ar, struct archive* aw) {
  int r;
  const void* buff;
  size_t size;
#if ARCHIVE_VERSION >= 3000000
  int64_t offset;
#else
  off_t offset;
#endif

  for(;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if(r == ARCHIVE_EOF) {
      return ARCHIVE_OK;
    }
    if(r != ARCHIVE_OK) {
      return (r);
    }
    r = archive_write_data_block(aw, buff, size, offset);
    if(r != ARCHIVE_OK) {
      printf("archive_write_data_block(): %s", archive_error_string(aw));
      return r;
    }
  }
}
