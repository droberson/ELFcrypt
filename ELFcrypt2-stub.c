#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/syscall.h>

#include "ELFcrypt.h"


static inline int memfd_create(const char *name, unsigned int flags) {
  return syscall(__NR_memfd_create, name, flags);
}

/* get_elf_size() -- returns length of ELF data for a file.
 *
 * TODO: validate ELF file
 *       figure out actual header size of ELF (malloc)
 */
size_t get_elf_size(const char *progname) {
  int fd;
  void *ELFheaderdata;
  Elf64_Ehdr *ELFheader;
  size_t elfsize;


  ELFheaderdata = malloc(64);

  fd = open(progname, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Failed to open input file %s: %s\n",
            progname,
            strerror(errno));
    fprintf(stderr, "Exiting.\n");

    exit(EXIT_FAILURE);
  }

  read(fd, ELFheaderdata, 64);
  ELFheader = (Elf64_Ehdr *)ELFheaderdata;

  elfsize = ELFheader->e_shoff + (ELFheader->e_shnum * ELFheader->e_shentsize);

  close(fd);
  free(ELFheaderdata);

  return elfsize;
}


int main(int argc, char *argv[], char *envp[]) {
  int fd;
  int in;
  size_t offset;
  size_t filesize;
  unsigned char *key;
  void *program;

  filesize = get_file_size(argv[0]);
  offset = get_elf_size(argv[0]);

  in = open(argv[0], O_RDONLY);
  if (in == -1) {
    fprintf(stderr, "Failed to open input file %s: %s\n",
      argv[0],
      strerror(errno));
    exit(EXIT_FAILURE);
  }

  program = mmap(0,
                 filesize,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE,
                 in,
                 0);
  if (program == MAP_FAILED) {
    fprintf(stderr, "Unable to mmap %s: %s\n", argv[0], strerror(errno));
    exit(EXIT_FAILURE);
  }

  program += offset;

  key = (unsigned char *)get_password();
  if (rc4(program, filesize - offset, key) == 1) {
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  fd = memfd_create("asdf", 1);

  if (write(fd, program, filesize - offset) != filesize - offset) {
    fprintf(stderr, "Failed to write %ld bytes: %s\n", filesize - offset, strerror(errno));
    exit(EXIT_FAILURE);
  }

  fexecve(fd, argv, envp);
  close(fd);

  return EXIT_SUCCESS;
}

