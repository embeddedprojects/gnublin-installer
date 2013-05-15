#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "archive.h"
#include "disk.h"
#include "cmdparser.h"

using namespace std;

void help();
void copy_device(FILE* in, FILE* out, long long kbytes, bool verbose);

int main(int argc, char* argv[]) {
  if(argc == 1) {
    help();
    exit(0);
  }

  CommandParser* p = new CommandParser(argc, argv);
  bool verbose = p->hasFlag("verbose", "v");

  // get action
  string action(argv[1]);
  if(action == "install" || action == "i") {
    cout << "Install" << endl;
    string dest, bootl, kernel, rootfs;
    bool repart = false;
    int bsize;

    // get parameter
    dest = p->getParameter("device", "d");
    bootl = p->getParameter("bootloader", "b");
    kernel = p->getParameter("kernel", "k");
    rootfs = p->getParameter("rootfs", "r");
    bsize = atoi(p->getParameter("bsize", "s").c_str());
    if(bsize == 0) {
      bsize = 8;
    }
    if(bsize < 4) {
      bsize = 4;
    }

    if(p->hasFlag("repartition", "p")) {
      repart = true;
    }

    // no destination given
    if(dest.empty()) {
      cout << "Error: no destination given!" << endl;
      exit(1);
    }

    // nothing to do
    if(bootl.empty() && kernel.empty() && rootfs.empty() && !repart) {
      cout << "Nothing to do. Exiting." << endl;
      exit(0);
    }

    // verbose output details
    if(verbose) {
      cout << "Device: \t" << dest << endl;
      if(!bootl.empty()) {
        cout << "Bootloader: \t" << bootl << endl;
      }
      if(!kernel.empty()) {
        cout << "Kernel: \t" << kernel << endl;
      }
      if(!rootfs.empty()) {
        cout << "RootFS: \t" << rootfs << endl;
      }
      cout << "Bootloader size: " << bsize << "MB" << endl;
      cout << "Repartition: " << (repart ? "Yes" : "No") << endl;
    }

    // unmounting partitions
    int c;
    for(c = 0; c < 2; c++) {
      string part = dest + (char)(c + 1 + '0');
      if(is_mounted(part.c_str())) {
        if(verbose) {
          cout << "Unmounting '" << part << "'" << endl;
        }
        unmount_partition(get_mountpoint(part.c_str()));

        if(is_mounted(part.c_str())) {
          cout << "Error: can not unmount '" << part << "'" << endl;
          exit(1);
        }
      }
    }

    // repartition
    if(repart) {
      if(verbose) {
        cout << "Creating partitions (bootloader partition: " << bsize << "MB)" << endl;
      }
      create_partitions(dest.c_str(), bsize * 1024 * 1024);
    }

    // write bootloader
    if(!bootl.empty()) {
      string boot_part = dest + "2";

      if(verbose) {
        cout << "Writing bootloader to partition '" << boot_part << "'" << endl;
      }
      dd(bootl.c_str(), boot_part.c_str(), 512);
    }

    // mounting partition
    if(verbose) {
      cout << "Mounting partition" << endl;
    }
    mkdir("linux", 0777);
    string linux_part = dest + "1";
    mount_partition(linux_part.c_str(), "linux");

    if(!is_mounted(linux_part.c_str())) {
      cout << "Error: can not mount partition." << endl;
      exit(1);
    }

    // extract rootfs
    if(!rootfs.empty()) {
      if(verbose) {
        cout << "Extracting RootFS" << endl;
      }
      extract_archive(rootfs.c_str(), "linux");
    }

    // copy kernel
    if(!kernel.empty()) {
      if(verbose) {
        cout << "Copying kernel" << endl;
      }
      copy_file(kernel.c_str(), "linux/zImage");
    }

    // unmounting
    if(verbose) {
      cout << "Unmounting partition" << endl;
    }
    unmount_partition("linux");

    //syncing
    if(verbose) {
      cout << "Syncing card" << endl;
    }
    sync_card();

    cout << "Installation successful!" << endl;

  } else if(action == "backup" || action == "b") {
    cout << "Backup" << endl;
    string device, file;
    bool verbose = p->hasFlag("verbose", "v");

    device = p->getParameter("device", "d");
    file = p->getParameter("out", "o");

    // check if output file and device are given
    if(device.empty() || file.empty()) {
      cout << "Error: device and output file must be given!" << endl;
      exit(1);
    }

    long dev_size = get_device_size(device.c_str()) / 1024;

    if(dev_size == 0) {
      cout << "Error: device size is 0" << endl;
      exit(1);
    }

    FILE* in = fopen(device.c_str(), "rb");
    if(in == NULL) {
      cout << "Error: can not open device" << endl;
      exit(1);
    }

    FILE* out = fopen(file.c_str(), "wb");
    if(out == NULL) {
      fclose(in);
      cout << "Error: can not open file '" << file << "'" << endl;
      exit(1);
    }

    if(verbose) {
      cout << dev_size << " kb to copy" << endl;
    }

    if(verbose) {
      cout << "Creating backup" << endl;
    }
    copy_device(in, out, dev_size, verbose);

    cout << "Creating backup was successful" << endl;
    exit(0);
  } else if(action == "restore" || action == "r") {
    cout << "Restore" << endl;

    string device, file;
    bool verbose = p->hasFlag("verbose", "v");

    device = p->getParameter("device", "d");
    file = p->getParameter("in", "i");

    // check if output file and device are given
    if(device.empty() || file.empty()) {
      cout << "Error: device and input file must be given!" << endl;
      exit(1);
    }


    // check if mounted
    int c;
    for(c = 0; c < 2; c++) {
      string part = device + (char)(c + 1 + '0');
      if(is_mounted(part.c_str())) {
        if(verbose) {
          cout << "Unmounting '" << part << "'" << endl;
        }
        unmount_partition(get_mountpoint(part.c_str()));
        if(is_mounted(part.c_str())) {
          cout << "Error: can not unmount '" << part << "'" << endl;
          exit(1);
        }
      }
    }

    long dev_size = get_device_size(device.c_str()) / 1024;

    if(dev_size == 0) {
      cout << "Error: device size is 0" << endl;
      exit(1);
    }

    FILE* out = fopen(device.c_str(), "wb");
    if(out == NULL) {
      cout << "Error: can not open device" << endl;
      exit(1);
    }

    FILE* in = fopen(file.c_str(), "rb");
    if(in == NULL) {
      fclose(out);
      cout << "Error: can not open file '" << file << "'" << endl;
      exit(1);
    }

    if(verbose) {
      cout << dev_size << " kb to copy" << endl;
    }

    if(verbose) {
      cout << "Restoring backup" << endl;
    }
    copy_device(in, out, dev_size, verbose);

    cout << "Restoring backup was successful" << endl;
    exit(0);

  } else if(action == "version" || action == "v") {
    cout << VERSION << endl;
  } else {
    help();
    exit(0);
  }

}

void copy_device(FILE* in, FILE* out, long long kbytes, bool verbose) {
  int blocksize = 1024 * 1024;
  char* buffer = (char*)malloc(blocksize);
  unsigned long read, total = 0;
  long long dev_size = kbytes;

  //long start_time = time(NULL);
  int last_ten = 0;

  while(!feof(in)) {
    read = fread(buffer, blocksize, 1, in);
    fwrite(buffer, blocksize, 1, out);
    total++;

    // calculate remaining time
    //long time_delta = time(NULL) - start_time;
    int percent = (int)(100.f * total * (blocksize / 1024.f)) / dev_size;

    if(percent != 0) {
      //long total_time = (100 * time_delta / percent);
      //long remaining_time = (time_delta / percent) * (100 - percent);
      if(verbose) {
        if((int)(percent / 2) != last_ten) {
          last_ten = (int)(percent / 2);
          cout << "." << flush;
        }
      }
    }
  }

  free(buffer);
  cout << endl;
}

void help() {
  cout << endl;
  cout << "GNUBLIN Installer " << VERSION << " Commandline \t\t Copyright (c) 2012 Michael Schwarz" << endl << endl;
  cout << "Usage: \t gnublin-cmdline <action> <parameter 1> <parameter N>" << endl << endl;
  cout << "<Action>" << endl;
  cout << "  install \tInstall GNUBLIN system" << endl;
  cout << "  backup \tBackup existing system to file" << endl;
  cout << "  restore \tRestore system from backup file" << endl;
  cout << "  version \tShows version of program" << endl << endl;
  cout << "<Parameter> " << endl;
  //cout << "--help -h \tShow this help" << endl;
  cout << "  -d \t--device \tSelect device (e.g. /dev/sda)" << endl;
  cout << "  -p \t--repartition \tCreate new partition table" << endl;
  cout << "  -b \t--bootloader \tSelect bootloader" << endl;
  cout << "  -k \t--kernel \tSelect kernel" << endl;
  cout << "  -r \t--rootfs \tSelect RootFS" << endl;
  cout << "  -s \t--bsize \tSelect Bootloader size (in MB)" << endl;
  cout << "  -i \t--in \t\tSelect file to restore from" << endl;
  cout << "  -o \t--out \t\tSelect file to backup to" << endl;
  cout << "  -v \t--verbose \tVerbose output" << endl;
  cout << endl;
  cout << "Examples:" << endl;
  cout << "Full-Install, repartition device with 4MB partition for bootloader" << endl;
  cout << "\tgnublin-cmdline install -d /dev/sdj -p -b apex.bin -k zImage -r rootfs.tar.gz -s 4" << endl;
  cout << "Backup existing installation" << endl;
  cout << "\tgnublin-cmdline backup --device /dev/sdj --out backup.img" << endl;
}
