#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define ENTRY ((unsigned char *)0x400000)
#define CRYPTED __attribute__((section(".crypted")))


char *get_password() {
  int i = 0;
  char *key;
  char keyconfirm[256];

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


int rc4(unsigned char *data, size_t size, const unsigned char *key) {
  int i;
  int rc4i;
  int rc4j;
  unsigned char rc4s[256];
  unsigned int tmp;

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


void ELFdecrypt() {
  int section_length;
  int crypted_section;
  char *key;
  unsigned char *ptr;
  unsigned char *ptr2;
  size_t pagesize;
  uintptr_t pagestart;
  int size;


  key = getenv("ELFCRYPT");
  if (key == NULL)
    key = get_password();

  /* retrieve crypted section offset and size stored by ELFcrypt */
  crypted_section = *((int *)(ENTRY + 0x09));
  section_length = *((short *)(ENTRY + 0x0d));

  /* Calculate offsets and sizes */
  ptr = ENTRY + crypted_section;
  ptr2 = ENTRY + crypted_section + section_length;
  pagesize = sysconf(_SC_PAGESIZE);
  pagestart = (uintptr_t)ptr & -pagesize;
  size = (ptr2 - (unsigned char *)pagestart);

  if (mprotect((void *)pagestart, size, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
    fprintf(stderr, "mprotect(): %s\n", strerror(errno));
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  /* decrypt using specified key */
  rc4(ENTRY + crypted_section, section_length, (unsigned char *)key);

  if (mprotect((void *)pagestart, size, PROT_READ | PROT_EXEC) < 0) {
    fprintf(stderr, "mprotect2\n");
    fprintf(stderr, "Exiting.\n");
    exit(EXIT_FAILURE);
  }

  /* erase key */
  memset(key, 0, strlen(key));
}
