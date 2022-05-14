#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>

/* Print message with given FMT format like in printf and exit program
 * with error code 1.  If FMT string ends with ':' then after message
 * print last system or stdlib error.  Implementation is shamelessly
 * stollen from Suckless "dwm" program source code. */
void die(char *fmt, ...);

/*
 * In SRC string replace all occurrences of FROM string with TO string
 * and save result in DEST string of MAX size.  All strings are
 * null-terminated.  Return 0 on success, return <0 on error.
 */
int strnrep(char *src, char *from, char *to, char *dest, size_t max);

#endif	/* UTIL_H_ */
