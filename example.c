/* Example program using ELFcrypt
 *
 * Include ELFcrypt.h and prefix functions you'd like crypted with CRYPTED.
 * Please also remember to add a call to ELFdecrypt() somewhere in your
 * program before the protected functions are used.
 *
 * Your program must then be ran through ELFcrypt:
 *
 * $ ./ELFcrypt /path/to/program -o output
 */

#include <stdio.h>

/* include ELFcrypt.h for the required macros and functions
 */
#include "ELFcrypt.h"

/* Prefix your functions with CRYPTED to add them to the .crypted section
 */
CRYPTED int crypted_main(int argc, char *argv[]) {
  printf("This function was crypted\n");

  return 100;
}


int main(int argc, char *argv[]) {
  ELFdecrypt(); /* you must call this function and provide a valid
		   password before the functions you've placed into the
		   .crypted section will work */
  return crypted_main(argc, argv);
}
