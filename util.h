#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* Print message with given FMT format like in printf and exit program
 * with error code 1.  If FMT string ends with ':' then after message
 * print last system or stdlib error.  Implementation is shamelessly
 * stollen from Suckless "dwm" program source code. */
void die(char *fmt, ...);

#endif	/* UTIL_H_ */

#ifdef IMPLEMENTATION

void
die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}
	exit(1);
}

#endif	/* IMPLEMENTATION */
