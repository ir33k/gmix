#include <string.h>

#include "parse.h"

int
parse__eol(char *str)
{
	return str[strlen(str)-1] == '\n';
}

LT
parse__lt(LT prev, char *buf)
{
	/* If previous Line type was a preformatted text ten all next
	 * lines are should also be preformatted unless we find ending
	 * line.  Then we treate it like empty line.  With that logic
	 * you can find opening of preformatted text when previous LT
	 * wasn't LT_PRE and now is, and closing of preformatted text
	 * when previous LT was LT_PRE and now is LT_BR. */
	if (prev == LT_PRE) {
		return strncmp(buf, "```",  3) == 0 ? LT_BR : LT_PRE;
	}

	if (buf[0] == '\n') return LT_BR;
	if (buf[0] == '>')  return LT_Q;
	if (strncmp(buf, "# ",   2) == 0) return LT_H1;
	if (strncmp(buf, "## ",  3) == 0) return LT_H2;
	if (strncmp(buf, "### ", 4) == 0) return LT_H3;
	if (strncmp(buf, "=> ",  3) == 0) return LT_URI;
	if (strncmp(buf, "```",  3) == 0) return LT_PRE;
	if (strncmp(buf, "* ",   2) == 0) return LT_LI;

	return LT_P;
}
