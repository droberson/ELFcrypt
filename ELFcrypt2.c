#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ELFcrypt.h"


/* main() -- Encrypt input file using RC4. Write output to a file.
 */
int main(int argc, char *argv[]) {
  int             in;
  int             out;
  size_t          filesize;
  void            *program;
  unsigned char   *key;


  printf("ELFcrypt2 by @dmfroberson\n");

  if (argc != 3)
    fatal("usage: %s <input program> <output filename>\n", argv[0]);

  in = open(argv[1], O_RDONLY);
  if (in == -1)
    fatal("Failed to open input file %s: %s\n", argv[1], strerror(errno));

  out = open(argv[2], O_WRONLY | O_CREAT, 0755);
  if (out == -1)
    fatal("Failed to open output file %s: %s\n", argv[2], strerror(errno));

  filesize = get_file_size(argv[1]);
  if (filesize == -1)
    fatal("Unable to determine size of input file.\n");

  program = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, in, 0);
  if (program == MAP_FAILED)
    fatal("Unable to mmap %s: %s\n", argv[1], strerror(errno));

  key = (unsigned char *)get_password();
  if (rc4(program, filesize, key) == -1)
    fatal("Failed to encrypt input file.\n");

  if (write(out, program, filesize) != filesize)
    fatal("Failed to write to output file %s: %s\n", argv[2], strerror(errno));

  close(in);
  close(out);

  return EXIT_SUCCESS;
}

