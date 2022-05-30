#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"

void
die(char *fmt, ...)
{
	va_list ap;

	printf("ERR: ");

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

int
flagged(int list, int flag)
{
	return (list & flag) == flag;
}
