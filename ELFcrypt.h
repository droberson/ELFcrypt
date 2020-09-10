#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#define ENTRY ((unsigned char *)0x400000)
#define CRYPTED __attribute__((section(".crypted")))


/* fatal() -- Prints a message and exits with EXIT_FAILURE
 *
 * Args:
 *     fmt - va_args-style format strings (like printf)
 *
 * Returns:
 *     Nothing.
 */
void fatal(char *fmt, ...) {
  va_list       ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  exit(EXIT_FAILURE);
}


/* is_valid_elf() -- Determine if data is a valid ELF file.
 *
 * Args:
 *     header - ELF header.
 *
 * Returns:
 *     1 if ELF magic checks are successful.
 *     0 if the magic bytes do not match.
 */
int is_valid_elf(Elf64_Ehdr *header) {
  if (!header)
    return 0;

  if (header->e_ident[EI_MAG0] != ELFMAG0 ||
      header->e_ident[EI_MAG1] != ELFMAG1 ||
      header->e_ident[EI_MAG2] != ELFMAG2 ||
      header->e_ident[EI_MAG3] != ELFMAG3)
    return 0;

  return 1;
}


/* get_elf_size() -- Calculate size of ELF data within a file.
 *
 * Args:
 *     progname - ELF file
 *
 * Returns:
 *     size of ELF data if successful.
 *     -1 if an error occurred.
 *
 * Note: if an error occurs, errno is set appropriately.
 */
size_t get_elf_size(const char *progname) {
  int           fd;
  void          *ELFheaderdata;
  Elf64_Ehdr    *ELFheader;
  size_t        elfsize;


  ELFheaderdata = malloc(64);

  fd = open(progname, O_RDONLY);
  if (fd == -1) {
    free(ELFheaderdata);
    errno = ENOENT;
    return -1;
  }

  read(fd, ELFheaderdata, 64);
  ELFheader = (Elf64_Ehdr *)ELFheaderdata;

  if (is_valid_elf(ELFheader) == 0) {
    errno = ENOEXEC;
    return -1;
  }

  elfsize = ELFheader->e_shoff + (ELFheader->e_shnum * ELFheader->e_shentsize);

  close(fd);
  free(ELFheaderdata);

  return elfsize;
}


/* get_elf_section() -- Get address of ELF section within data.
 *
 * Args:
 *     data    - ELF data in memory
 *     section - section name to search for
 *
 * Returns:
 *     Address of section if successful.
 *     NULL if section does not exist.
 */
Elf64_Shdr *get_elf_section(void *data, const char *section) {
  int           i;
  char          *offset;


  /* Populate ELF headers and section headers from mmapped file */
  Elf64_Ehdr *ELFheader = (Elf64_Ehdr *)data;
  Elf64_Shdr *sectionheader = (Elf64_Shdr *)(data + ELFheader->e_shoff);
  Elf64_Shdr *next = &sectionheader[ELFheader->e_shstrndx];

  /* Make sure this is a valid ELF before proceeding */
  if (is_valid_elf(ELFheader) == 0)
    fatal("Input file is not a valid ELF file.\n");

  offset = data + next->sh_offset;

  /* Search for "section" and return address of matching section header */
  for (i = 0; i < ELFheader->e_shnum; i++) {
    if (!strcmp((char *)offset + sectionheader[i].sh_name, section)) {
      return &sectionheader[i];
    }
  }

  return NULL;
}


/* get_file_size() -- Determine the size of a file.
 *
 * Args:
 *     filename - Path to file.
 *
 * Returns:
 *     Size of file in bytes on success.
 *     -1 if something went wrong.
 */
size_t get_file_size(const char *filename) {
  struct stat     s;

  if (stat(filename, &s) == -1) {
    fprintf(stderr, "Failed to stat file %s: %s\n", filename, strerror(errno));
    return -1;
  }

  return s.st_size;
}


/* get_password() -- Prompt user for a password and password confirmation.
 *
 * Args:
 *     None
 *
 * Returns:
 *     Pointer to string containing the password.
 *
 * Note: this is limited to 256 byte passwords because this is the maximum
 * key length RC4 is able to use.
 */
char *get_password() {
  int             i = 0;
  char            *key;
  char            keyconfirm[256];

  do {
    if (i) {
      printf("Passwords do not match\n");
      sleep(3);
    }

    if ((key = getpass("Enter passphrase: ")) == NULL) {
      printf("Bad password.\n");
      continue;
    }

    strncpy(keyconfirm, key, sizeof(keyconfirm));

    if ((key = getpass("Confirm passphrase: ")) == NULL) {
      printf("Bad password.\n");
      continue;
    }

    i = 1;
  } while (strcmp(key, keyconfirm));

  /* zero out key from memory */
  memset(keyconfirm, 0, sizeof(keyconfirm));

  return key;
}


/* rc4() -- Encrypt data using RC4 encryption algorithm.
 *
 * Args:
 *     data - Data to encrypt
 *     size - Length of data
 *     key  - Passphrase to encrypt data with.
 *
 * Returns:
 *     0 if successful.
 *     1 if unsuccessful.
 */
int rc4(unsigned char *data, size_t size, const unsigned char *key) {
  int           i;
  int           rc4i;
  int           rc4j;
  unsigned char rc4s[256];
  unsigned int  tmp;

  if (strlen((char *)key) > sizeof(rc4s)) {
    fprintf(stderr, "Key must be under %ld bytes\n", sizeof(rc4s));
    return 1;
  }

  /* Key-scheduling algorithm */
  for (i = 0; i < sizeof(rc4s); i++)
    rc4s[i] = i;

  for (rc4i = 0, rc4j = 0; rc4i < sizeof(rc4s); rc4i++) {
    rc4j = (rc4j + rc4s[rc4i] + key[rc4i % strlen((char *)key)]) % sizeof(rc4s);

    /* swap s[i] and s[j] */
    tmp = rc4s[rc4j];
    rc4s[rc4j] = rc4s[rc4i];
    rc4s[rc4i] = tmp;
  }

  /* encrypt data */
  for (rc4i = 0, rc4j = 0, i = 0; i < size; i++) {
    rc4i = (rc4i + 1) % sizeof(rc4s);
    rc4j = (rc4j + rc4s[rc4i]) % sizeof(rc4s);

    /* swap s[i] and s[j] */
    tmp = rc4s[rc4j];
    rc4s[rc4j] = rc4s[rc4i];
    rc4s[rc4i] = tmp;

    tmp = rc4s[(rc4s[rc4i] + rc4s[rc4j]) % sizeof(rc4s)];
    data[i] ^= tmp;
  }

  return 0;
}


/* ELFdecrypt() -- Decrypt .crypted section of ELF file.
 *
 * Args:
 *     pass - If desired, pass the key in here. This is not very secure,
 *            but provides some obfuscation.
 *
 * Returns:
 *     Nothing
 *
 * Note: if the ELFCRYPT environment variable is set, this will attempt to use
 * its contents as the encryption key.
 */
void ELFdecrypt(char *pass) {
  int           section_length;
  int           crypted_section;
  char          *key;
  unsigned char *ptr;
  unsigned char *ptr2;
  size_t        pagesize;
  uintptr_t     pagestart;
  int           size;


  if (pass == NULL) {
    key = getenv("ELFCRYPT");
    if (key == NULL) {
      key = getpass("Enter passphrase: ");
    } else {
      unsetenv("ELFCRYPT");
    }
  } else {
    key = strdup(pass);
  }

  /* Retrieve crypted section offset and size stored by ELFcrypt */
  crypted_section = *((int *)(ENTRY + 0x09));
  section_length = *((short *)(ENTRY + 0x0d));

  /* Calculate offsets and sizes */
  ptr = ENTRY + crypted_section;
  ptr2 = ENTRY + crypted_section + section_length;
  pagesize = sysconf(_SC_PAGESIZE);
  pagestart = (uintptr_t)ptr & -pagesize;
  size = (ptr2 - (unsigned char *)pagestart);

  if (mprotect((void *)pagestart, size, PROT_READ | PROT_WRITE | PROT_EXEC) == -1)
    fatal("mprotect(): %s\n", strerror(errno));

  /* decrypt using specified key */
  rc4(ENTRY + crypted_section, section_length, (unsigned char *)key);

  if (mprotect((void *)pagestart, size, PROT_READ | PROT_EXEC) < 0)
    fatal("mprotect(): %s\n", strerror(errno));

  /* erase key */
  memset(key, 0, strlen(key));
}

