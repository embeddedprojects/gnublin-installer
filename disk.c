/*  disk.c
 *
 *  Author:    Michael Schwarz
 *  Copyright (C) 2011 Michael Schwarz
 *
 * Funtions for creating partitions, mounting, unmounting and copying
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <parted/parted.h>
#include <sys/mount.h>
#include <unistd.h>

#include "disk.h"

#define MAX_DRIVES 128
#define MAX_PARTITIONS 8

#define BOOT_SECTOR_SIZE (1024*1024*16)
#define FILE_SYSTEM "ext2"

char command2[1000];

/**
 * Checks if the given partition is mounted
 *
 * */
int is_mounted(const char* partition) {
  char buffer[256];
  FILE* f;
  int status = 0;

  f = fopen("/proc/mounts", "r");
  if(f == NULL) {
    return -1;
  }

  while(!feof(f)) {
    if(fgets(buffer, 256, f) == NULL) {
      //printf("get_mountpoint: error read /proc/mounts\n");
      //return -1;
    }
    if(strncmp(buffer, partition, strlen(partition)) == 0 && buffer[strlen(partition)] == ' ') {
      status = 1;
      break;
    }
  }

  return status;
}


char* get_mountpoint(const char* source) {
  static char buffer[256];
  FILE* f;

  f = fopen("/proc/mounts", "r");
  if(f == NULL) {
    return NULL;
  }

  while(!feof(f)) {
    if(fgets(buffer, 256, f) == NULL) {
      printf("get_mountpoint: error read /proc/mounts\n");
      //return (char*) "ERROR";
    }
    if(strncmp(buffer, source, strlen(source)) == 0 && buffer[strlen(source)] == ' ') {
      // get mount point
      int end = strlen(source) + 1;
      while(buffer[end] != ' ' && buffer[end] != 0) {
        end++;
      }
      buffer[end] = 0;
      return &buffer[strlen(source) + 1];
    }
  }
  return NULL;
}

/**
 * (internal) Timer for file system creation progress
 *
 */
void create_ext2_timer(PedTimer* timer, void* context) {
  //printf(".");
}


/**
 * Creates the boot- and the linux partition and formats the linux
 * partition with an ext2 filesystem
 *
 */
int create_partitions(const char* dev, unsigned long bootsector_size) {
  PedDisk* disk;
  PedDevice* device;
  PedPartition* boot_part;
  PedPartition* linux_part;
  PedPartition* kernel_part;
  PedFileSystemType* fs_type;
  PedTimer* timer;

  printf("create partitions started! ped_device_get...\n dev = %s\n", dev);
  // get device from string e.g. "/dev/sdd"
  device = ped_device_get(dev);
  if(device == NULL) {
    return 0;
  }

  printf("ped_device_get done! create new partition table...\n");
  // create new partition table
  disk = ped_disk_new_fresh(device, ped_disk_type_get("msdos"));
  if(disk == NULL) {
    ped_device_destroy(device);
    return 0;
  }

  printf("partition table done! get file system type...\n");
  // get file system type (ext2)
  fs_type = ped_file_system_type_get(FILE_SYSTEM);


  printf("get file system type done! create partitions...\n");
  // create partitions
  boot_part = ped_partition_new(disk, PED_PARTITION_NORMAL, fs_type, 0, bootsector_size / device->sector_size);
  kernel_part = ped_partition_new(disk, PED_PARTITION_NORMAL, fs_type, bootsector_size / device->sector_size, (2 * (bootsector_size / device->sector_size)));
  linux_part = ped_partition_new(disk, PED_PARTITION_NORMAL, fs_type, (2 * (bootsector_size / device->sector_size)), device->length - 1);

  printf("create partitions done! add partitions to partition table...\n");
  // add partitions to partition table
  PedConstraint* constraint = ped_constraint_any(device);
  ped_disk_add_partition(disk, kernel_part, constraint);
  ped_disk_add_partition(disk, boot_part, constraint);
  ped_disk_add_partition(disk, linux_part, constraint);
  ped_constraint_destroy(constraint);

  printf("add partitions done! create timer...\n");
  // create timer
  timer = ped_timer_new(create_ext2_timer, NULL);

  printf("create timer done! create filesystems...\n");
  // create filesystem
  ped_file_system_create(&kernel_part->geom, fs_type, timer);
  ped_file_system_create(&linux_part->geom, fs_type, timer);

  printf("create filesystem done! commit to hardware...\n");
  // commit to hardware
  ped_disk_commit_to_dev(disk);
  ped_disk_commit_to_os(disk);

  //change boot partition id
  printf("create filesystem done! switch boot partition id to \"DF\" ...\n");


  change_to_bootit(dev);
  //  sprintf(command2, "sfdisk -c %s 2 df",dev);
  //  system(command2);


  //tune2fs
  printf("switch partition id done! tune2fs (ext3&lable Gnublin) ...\n");
  if(strcmp(dev, "/dev/mmcblk0") == 0) {
    sprintf(command2, "tune2fs -j -L Kernel %sp1", dev);
  } else {
    sprintf(command2, "tune2fs -j -L Kernel %s1", dev);
  }
  if(system(command2) != 0) {
    printf("create_partitions: error call system command Kernel tune2fs...!\n");
    return -1;
  }

  printf("Gnublin tune2fs done!! tune2fs (ext4&lable Gnublin) ...\n");
  if(strcmp(dev, "/dev/mmcblk0") == 0) {
    sprintf(command2, "tune2fs -O extents,uninit_bg,dir_index -L Gnublin %sp3", dev);
  } else {
    sprintf(command2, "tune2fs -O extents,uninit_bg,dir_index -L Gnublin %s3", dev);
  }
  if(system(command2) != 0) {
    printf("create_partitions: error call system command Gnublin tune2fs...!\n");
    return -1;
  }


  printf("tune2fs done! create partitions finished!\n");



  return 1;
}

/**
 *Changes the Partition id of the bootloader partition to "df"(BootIt)
 *
 *
 */
int change_to_bootit(const char* dev) {
  printf("File to change to bootit id: %s\n", dev);
  char puffer[1];
  puffer[0] = 0xdf; //df = boot it id
  int fd;
  if(-1 == (fd = open(dev, O_RDWR | O_NONBLOCK))) {
    printf("Error open file: %s\n", dev);
    return -1;
  }
  lseek(fd, 466L, 0);
  if(write(fd, puffer, 1) != 1) {
    printf("change_to_bootit: write error!\n");
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

/**
 * Mounts an ext2 partition to the given directory
 *
 */
int mount_partition(const char* partition, const char* mount_point) {

  char command[1000];

  sprintf(command, "mount %s %s", partition, mount_point);
  if(system(command) != 0) {
    printf("tar: ERROR call tar systemcall...!\n");
    return -1;
  }
  return 0; //mount(partition, mount_point, "ext4", 0, NULL);
}


/**
 * Umounts the given directory
 *
 */
int unmount_partition(const char* mount_point) {
  if(mount_point == NULL) {
    return 1;
  }
  return umount(mount_point);
}


/**
 * Returns 1 if the user is root, otherwise 0
 *
 */
int is_root() {
  return geteuid() == 0;
}


/**
 * Returns an array of partitions of the given device
 *
 *
 */
Partition** get_partitions(const char* dev) {
  PedDevice* device;
  PedDisk* disk;
  PedPartition* part;
  Partition** partitions = (Partition**)malloc(MAX_PARTITIONS * sizeof(Partition*));
  int part_nr = 0;

  device = ped_device_get(dev);
  if(device == NULL) {
    return NULL;
  }

  disk = ped_disk_new(device);
  if(disk == NULL) {
    ped_device_destroy(device);
    return NULL;
  }

  for(part = ped_disk_next_partition(disk, NULL); part; part = ped_disk_next_partition(disk, part)) {
    if(part->num < 0) {
      continue;
    }
    partitions[part_nr] = (Partition*)malloc(sizeof(Partition));
    partitions[part_nr]->num = part->num;
    partitions[part_nr]->start = part->geom.start;
    partitions[part_nr]->length = part->geom.length;
    part_nr++;
  }

  partitions[part_nr] = NULL;

  ped_disk_destroy(disk);
  ped_device_destroy(device);

  return partitions;
}

/**
 * Returns an array of devices
 *
 */
Drive** get_devices() {
  PedDevice* device = NULL;
  Drive** devs = (Drive**)malloc(MAX_DRIVES * sizeof(Drive*));
  int drives = 0;

  ped_device_probe_all();
  while((device = ped_device_get_next(device))) {
    devs[drives] = (Drive*)malloc(sizeof(Drive));
    devs[drives]->length = device->length;
    devs[drives]->sector_size = device->sector_size;
    devs[drives]->name = (char*)malloc((strlen(device->path) + 1) * sizeof(char));
    strcpy(devs[drives]->name, device->path);
    devs[drives]->model = (char*)malloc((strlen(device->model) + 1) * sizeof(char));
    strcpy(devs[drives]->model, device->model);
    drives++;
  }
  devs[drives] =  NULL;
  return devs;
}

/**
 * Returns the size of a device
 *
 */
unsigned long long get_device_size(const char* dev) {
  PedDevice* device = ped_device_get(dev);
  if(device == NULL) {
    return 0;
  }
  return device->length * device->sector_size;
}

/**
 * Makes a size in bytes human-readable
 *
 */
void format_size(char* buffer, unsigned long long size) {
  int unit = 0;
  const char* units[] = {"MB", "GB", "TB", "PB"};

  size /= 1024 * 1024;
  double s = size;
  while(s >= 1024) {
    s /= 1024;
    unit++;
  }

  sprintf(buffer, "%.1f %s", s, units[unit]);
}

unsigned long dd(const char* input, const char* output, unsigned long blocksize) {
  FILE* in = fopen(input, "rb");
  if(in == NULL) {
    return 0;
  }

  FILE* out = fopen(output, "wb");
  if(out == NULL) {
    return 0;
  }

  char* buffer = (char*)malloc(blocksize);
  unsigned long total = 0;

  while(!feof(in)) {
    if(fread(buffer, blocksize, 1, in) != blocksize) {
      //printf("dd: Read error!\n");
      //return -1;
    }
    fwrite(buffer, blocksize, 1, out);
    total += blocksize;
  }

  fclose(in);
  fclose(out);

  free(buffer);

  return total;
}


unsigned long copy_file(const char* input, const char* output) {
  FILE* in = fopen(input, "rb");
  if(in == NULL) {
    return 0;
  }

  FILE* out = fopen(output, "wb");
  if(out == NULL) {
    return 0;
  }

  char buffer[512];
  unsigned long read, total = 0;
  while(!feof(in)) {
    read = fread(buffer, 1, 512, in);
    fwrite(buffer, 1, read, out);
    total += read;
  }

  fclose(in);
  fclose(out);

  return total;
}

void sync_card() {
  sync();
}



