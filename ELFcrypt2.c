#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ELFcrypt.h"


int main(int argc, char *argv[]) {
  int in;
  int out;
  size_t filesize;
  void *program;
  unsigned char *key;


  printf("ELFcrypt2 by @dmfroberson\n");

  if (argc != 3) {
    fprintf(stderr, "usage: %s <input program> <output filename>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  in = open(argv[1], O_RDONLY);
  if (in == -1) {
    fprintf(stderr, "Failed to open input file %s: %s\n",
      argv[1],
      strerror(errno));
    exit(EXIT_FAILURE);
  }

  out = open(argv[2], O_WRONLY | O_CREAT, 0755);
  if (out == -1) {
    fprintf(stderr, "Failed to open output file %s: %s\n",
      argv[2],
      strerror(errno));
    exit(EXIT_FAILURE);
  }

  filesize = get_file_size(argv[1]);
  if (filesize == -1) {
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  program = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, in, 0);
  if (program == MAP_FAILED) {
    fprintf(stderr, "Unable to mmap %s: %s\n", argv[1], strerror(errno));
    exit(EXIT_FAILURE);
  }

  key = (unsigned char *)get_password();

  if (rc4(program, filesize, key) == -1) {
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  if (write(out, program, filesize) != filesize) {
    fprintf(stderr, "Failed to write %ld bytes to %s: %s\n",
      filesize,
      argv[2],
      strerror(errno));
    exit(EXIT_FAILURE);
  }

  close(in);
  close(out);

  return EXIT_SUCCESS;
}

