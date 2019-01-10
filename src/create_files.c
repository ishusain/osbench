// This is free and unencumbered software released into the public domain.
// For more information, see UNLICENSE.

#include "common/time.h"

#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// Note: The maximum number of files in a folder for different file systems:
//  - 65534 (FAT32)
//  - 4294967295 (NTFS, ext4)
#define NUM_FILES 65534

static double my_log2(double x) {
  static const double LOG2SCALE = 1.442695040888963;  // 1.0 / log(2.0);
  return log(x) * LOG2SCALE;
}

static int num_hex_chars(int max_int) {
  int num_bits = (int)ceil(my_log2((double)max_int));
  return (num_bits + 3) / 4;
}


static void to_hex(int x, char* str, const int str_len) {
  static const char TOHEX[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
  };

  str += str_len - 1;
  for (int i = 0; i < str_len; ++i) {
    *str-- = TOHEX[x & 15];
    x = x >> 4;
  }
}

static void create_file(int dirfd, const char* file_name) {
  static const char FILE_DATA[32] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    26, 27, 28, 29, 30, 31
  };

  int fd = openat(dirfd, file_name, O_WRONLY | O_CREAT);

  if (fd == -1) {
    fprintf(stderr, "*** Unable to create file \"%s\"\n", file_name);
    exit(1);
  }

  size_t off = 0;
  while (off < sizeof(FILE_DATA)) {
    off += write(fd, (const void *) &FILE_DATA[off], sizeof(FILE_DATA) - off);
  }

  if (-1 == fsync(fd)) {
    fprintf(stderr, "*** Unable to sync file \"%s\"\n", file_name);
    exit(1);
  }
  close(fd);
}

static void delete_file(int dirfd, const char* file_name) {
  const int no_flags = 0;
  unlinkat(dirfd, file_name, no_flags);
}



static char *file_names[NUM_FILES];

int main(int argc, const char** argv) {
  //if (argc != 2) {
  //  printf("Usage: %s root-folder\n", argv[0]);
  //  exit(1);
  //}
  char *rootpath = "/root/tmp";

  printf("Benchmark: Create/delete %d files...\n", NUM_FILES);
  fflush(stdout);


  int rootfd;
  DIR *dirp = opendir(rootpath);
  if (!dirp)
  {
    fprintf(stderr, "*** Unable to opend irectory \"%s\"\n", rootpath);
    exit(-1);
  }
  if ((rootfd = dirfd(dirp)) == -1)
  {
	  perror("open rootpath");
  }

  int hex_len = num_hex_chars(NUM_FILES -1);

  // Create a path string.
  for (size_t i = 0; i < NUM_FILES; i++)
  {
    size_t filename_len = num_hex_chars(NUM_FILES - 1);
    char* file_name = (char*)calloc(filename_len, 2);
    if (!file_name) {
      fprintf(stderr, "*** Out of memory!\n");
      exit(1);
    }

    to_hex(i, file_name, hex_len);
    file_names[i] = file_name;
  }

  const double t0 = get_time();

  for (int i = 0; i < NUM_FILES; ++i) {
    // Create the file.
    create_file(rootfd, file_names[i]);
  }

  for (int i = 0; i < NUM_FILES; ++i) {
    // Delete the file.
    delete_file(rootfd, file_names[i]);
  }

  double dt = get_time() - t0;

  printf("%f us / file\n", (dt * 1000000 / NUM_FILES));
  fflush(stdout);

  for(size_t i = 0; i < NUM_FILES; i++)
  {
	free(file_names[i]);
  }

  if(closedir(dirp) == -1)
  {
    perror("closedir");
    exit(-1);
  }

  return 0;
}

