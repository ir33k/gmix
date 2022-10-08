#ifndef STR_H_
#define STR_H_

#include <string.h>

/*
 * In SRC string replace all occurrences of FROM string with TO string
 * and save result in DEST string of MAX size.  All strings are
 * null-terminated.  Return 0 on success, return <0 on error.
 */
int str_nrep(char *src, char *from, char *to, char *dest, size_t max);

/*
 * Trim left side of STR string from spaces and tabs by returning
 * pointer to first non whitespace character.
 */
char *str_triml(char *str);

#endif	/* STR_H_ */
