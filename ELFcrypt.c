#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <elf.h>

#include "ELFcrypt.h"


/* Globals */
unsigned char *key;
char *outfile = "crypted";


/* is_valid_elf() -- returns 0 if ELF header is valid, 1 if not
 */
int is_valid_elf(Elf64_Ehdr *header) {
  if (!header)
    return 1;

  if (header->e_ident[EI_MAG0] != ELFMAG0 ||
      header->e_ident[EI_MAG1] != ELFMAG1 ||
      header->e_ident[EI_MAG2] != ELFMAG2 ||
      header->e_ident[EI_MAG3] != ELFMAG3)
    return 1;

  return 0;
}


/* get_elf_section()
 */
Elf64_Shdr *get_elf_section(void *data, const char *section) {
  int i;
  char *offset;

  /* Populate ELF headers and section headers from mmapped file */
  Elf64_Ehdr *ELFheader = (Elf64_Ehdr *)data;
  Elf64_Shdr *sectionheader = (Elf64_Shdr *)(data + ELFheader->e_shoff);
  Elf64_Shdr *next = &sectionheader[ELFheader->e_shstrndx];

  /* Make sure this is a valid ELF before proceeding */
  if (is_valid_elf(ELFheader) == 1) {
    fprintf(stderr, "Input file is not a valid ELF file.\n");
    fprintf(stderr, "Exiting.\n");

    exit(EXIT_FAILURE);
  }

  offset = data + next->sh_offset;

  /* Search for "section" and return address of matching section header */
  for (i = 0; i < ELFheader->e_shnum; i++) {
    if (!strcmp((char *)offset + sectionheader[i].sh_name, section)) {
      return &sectionheader[i];
    }
  }

  return NULL;
}


/* get_file_size() -- returns size of file in bytes
 */
size_t get_file_size(const char *filename) {
  struct stat s;

  if (stat(filename, &s) == -1) {
    fprintf(stderr, "Failed to stat file %s: %s\n", filename, strerror(errno));
    return -1;
  }

  return s.st_size;
}


/* ELFcrypt()
 */
int ELFcrypt(const char *filename, const char *outfile, const unsigned char *key) {
  int fd;
  int output;
  size_t filesize;
  void *program;
  Elf64_Shdr *crypted;


  /* calculate file size */
  filesize = get_file_size(filename);
  if (filesize == -1) {
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  /* open input and output files */
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Failed to open input file %s: %s\n",
	    filename,
	    strerror(errno));
    fprintf(stderr, "Exiting.\n");

    exit(EXIT_FAILURE);
  }

  output = open(outfile, O_WRONLY | O_CREAT, 0755);
  if (fd == -1) {
    fprintf(stderr, "Failed to open output file %s: %s\n",
	    outfile,
	    strerror(errno));
    fprintf(stderr, "Exiting.\n");

    exit(EXIT_FAILURE);
  }

  /* mmap ELF */
  program = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (program == MAP_FAILED) {
    fprintf(stderr, "Unable to mmap %s: %s\n", filename, strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* Get .crypted section of ELF file */
  crypted = get_elf_section(program, ".crypted");
  if (crypted == NULL) {
    fprintf(stderr, "No .crypted section found in %s\n", filename);
    fprintf(stderr, "Exiting.\n");
    unlink(outfile);
 
    exit(EXIT_FAILURE);
  }

  /* encrypt .crypted section using key */
  key = (unsigned char *)get_password();

  if (rc4(program + crypted->sh_offset, crypted->sh_size, (unsigned char *)key) == 1) {
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  /* store offset and size of .crypted section for future reference */
  *((int *)(program + 0x09)) = crypted->sh_offset;
  *((short *)(program + 0x0d)) = crypted->sh_size;

  /* write outfile */
  if (write(output, program, filesize) != filesize) {
    fprintf(stderr, "Failed to write %ld bytes to %s: %s\n",
	    filesize,
	    outfile,
	    strerror(errno));
    fprintf(stderr, "Exiting.\n");

    exit(EXIT_FAILURE);
  }

  /* cleanup */
  close(fd);
  close(output);

  return 0;
}


/* usage()
 */
void usage(const char *progname) {
  fprintf(stderr, "usage: %s <program> [-o <outfile>] [-h?]\n", progname);
  fprintf(stderr, "  -o <outfile> -- final resting place of crypted output. Default: %s\n", outfile);

  exit(EXIT_FAILURE);
}


/* main()
 */
int main(int argc, char *argv[]) {
  int ch;
  char *progname;

  progname = argv[0];

  printf("ELFcrypt by @dmfroberson\n\n");

  while((ch = getopt(argc, argv, "o:h?")) != -1) {
    switch(ch) {
    case 'o':
      outfile = optarg;
      break;
    case '?':
    case 'h':
    default:
      usage(progname);
      break;
    }
  }

  argc -= optind;
  argv += optind;

  /* check for required argument */
  if (!argv[0]) {
    usage(progname);
  }

  printf("Crypting .crypted section of %s, outputting to %s\n\n",
	 argv[0],
	 outfile);

  ELFcrypt(argv[0], outfile, key);

  return EXIT_SUCCESS;
}
