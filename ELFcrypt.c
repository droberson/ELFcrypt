#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

#include "ELFcrypt.h"


/* Global variables */
unsigned char   *key		= NULL;
char            *outfile	= "crypted";


/* ELFcrypt() -- Encrypts ELF file, writing encrypted results to an output file.
 *
 * Args:
 *     in  - Path to input ELF file.
 *     out - Path to output crypted ELF file.
 *     key - RC4 key to encrypt input file with.
 *
 * Returns:
 *    Nothing.
 */
void ELFcrypt(const char *in, const char *out, const unsigned char *key) {
  int           fd;
  int           output;
  size_t        filesize;
  void          *program;
  Elf64_Shdr    *crypted;


  /* Calculate file size */
  filesize = get_file_size(in);
  if (filesize == -1)
    fatal("Unable to calculate size of input file %s\n", in);

  /* Open input and output files */
  fd = open(in, O_RDONLY);
  if (fd == -1)
    fatal("Failed to open input file %s: %s\n", in, strerror(errno));

  output = open(out, O_WRONLY | O_CREAT, 0755);
  if (output == -1)
    fatal("Failed to open output file %s: %s\n", out, strerror(errno));

  /* mmap input file */
  program = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (program == MAP_FAILED)
    fatal("Unable to mmap %s: %s\n", in, strerror(errno));

  /* Get .crypted section of ELF file */
  crypted = get_elf_section(program, ".crypted");
  if (crypted == NULL) {
    unlink(out);
    fatal("No .crypted section found in %s\n", in);
  }

  /* Encrypt .crypted section using 'key' */
  if (rc4(program + crypted->sh_offset, crypted->sh_size, (unsigned char *)key) == 1)
    fatal("Failed to encrypt input file %s\n", in);

  /* Store offset and size of .crypted section for future reference. These
   * values will be used later by the ELFdecrypt() function. The
   * e_ident[EI_PAD] section provides a conveinient 7 byte location to store
   * these values in the ELF header.
   */
  *((int *)(program + 0x09)) = crypted->sh_offset;
  *((short *)(program + 0x0d)) = crypted->sh_size;

  /* Write outfile */
  if (write(output, program, filesize) != filesize)
    fatal("Failed to write to output file %s: %s\n", out, strerror(errno));

  /* Close file descriptors. Skipped munmap() because this happens
   * automatically when the program exits.
   */
  close(fd);
  close(output);
}


/* usage() -- Prints help menu and exits.
 *
 * Args:
 *     progname - String containing the name of the program.
 *
 * Returns:
 *     Nothing.
 */
void usage(const char *progname) {
  fprintf(stderr, "usage: %s <program> [-o <outfile>] [-k <key>] [-h?]\n", progname);
  fprintf(stderr, "  -o <outfile> -- final resting place of crypted output. Default: %s\n", outfile);
  fprintf(stderr, "  -k <key>     -- key to crypt ELF with. (bypasses getpass() routine)\n");

  exit(EXIT_FAILURE);
}


/* main()
 */
int main(int argc, char *argv[]) {
  int       ch;
  char      *progname = argv[0];


  printf("ELFcrypt by @dmfroberson\n\n");

  while((ch = getopt(argc, argv, "o:k:h?")) != -1) {
    switch(ch) {
    case 'o':
      outfile = optarg;
      break;
    case 'k':
      key = optarg;
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

  /* Check for required infile argument */
  if (!argv[0])
    usage(progname);

  printf("Crypting .crypted section of %s, outputting to %s\n\n",
	 argv[0],
	 outfile);

  if (key == NULL)
    key = (unsigned char *)get_password();

  ELFcrypt(argv[0], outfile, key);

  return EXIT_SUCCESS;
}
