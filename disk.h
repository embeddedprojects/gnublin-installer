#ifndef _DISK_H_
#define _DISK_H_

typedef struct _Partition {
  int num;
  unsigned long long start;
  unsigned long long length;
} Partition;

typedef struct _Drive {
  char* name;
  char* model;
  unsigned long long length;
  unsigned long sector_size;
} Drive;


int is_mounted(const char* partition);
int is_root();
Partition** get_partitions(const char* dev);
Drive** get_devices();
void format_size(char* buffer, unsigned long long size);
int create_partitions(const char* dev, unsigned long bootsector_size);
int mount_partition(const char* partition, const char* mount_point);
int unmount_partition(const char* mount_point);
void sync_card();
unsigned long copy_file(const char* input, const char* output);
unsigned long dd(const char* input, const char* output, unsigned long blocksize);
char* get_mountpoint(const char* source);
unsigned long long get_device_size(const char* dev);


#endif

