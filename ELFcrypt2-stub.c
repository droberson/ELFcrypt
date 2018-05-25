#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>

#include "ELFcrypt.h"


/* memfd_create prototype */
static inline int memfd_create(const char *name, unsigned int flags) {
  return syscall(__NR_memfd_create, name, flags);
}


/* main() -- Use the ELFappend technique to retrieve encrypted ELF then attempt
 *           to execute it in memory. I opted out of verbose error messages
 *           for this program to mask its intentions a little bit.
 */
int main(int argc, char *argv[], char *envp[]) {
  int             fd;
  int             in;
  size_t          offset;
  size_t          filesize;
  unsigned char   *key;
  void            *program;


  /* Calculate size of the stub + encrypted ELF */
  filesize = get_file_size(argv[0]);

  /* Calculate size of the stub */
  offset = get_elf_size(argv[0]);

  /* Open stub + encrypted ELF for reading, then mmap() it */
  in = open(argv[0], O_RDONLY);
  if (in == -1)
    return EXIT_FAILURE;

  program = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, in, 0);
  if (program == MAP_FAILED)
    return EXIT_FAILURE;

  /* Skip the stub. The encrypted data lies right after the stub. */
  program += offset;

  /* Attempt to decrypt the ELF using the key supplied by the user */
  key = (unsigned char *)getpass("Enter passphrase: ");
  if (rc4(program, filesize - offset, key) == 1)
    return EXIT_FAILURE;

  /* Some operating systems may not supply this function. This has only
   * been tested on modern Linux distributions (as of 2018). Alternatively,
   * you may be able to modify this to utilize a temporary file. We use the
   * memfd_create() system call here to avoid writes to the disk.
   */
  fd = memfd_create("asdf", 1);
  if (fd == -1)
    return EXIT_FAILURE;

  if (write(fd, program, filesize - offset) != filesize - offset)
    return EXIT_FAILURE;

  /* Attempt to execute decrypted ELF which is stored in memory. */
  close(in);
  munmap(program, filesize);
  fexecve(fd, argv, envp);
  close(fd);

  return EXIT_SUCCESS;
}

