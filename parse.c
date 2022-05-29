#include <stdio.h>
#include <string.h>

#include "parse.h"

PARSE
parse(PARSE state, char *line, size_t siz, FILE *fp)
{
	int     eol;		/* End Of Line, does line ends? */

	if (state == PARSE_END)
		return PARSE_ERR;

	/* Check if previous line ended.  Skip when parsing begun. */
	eol = state == PARSE_BEG ? 1 : line[strlen(line)-1] == '\n';

	if (fgets(line, siz, fp) == NULL)
		return PARSE_END;

	if (eol == 0)
		return PARSE_WIP;

	if (state == PARSE_NUL) {
		if (strncmp(line, "```", 3) == 0)
			return PARSE_ON;

		return PARSE_NUL;
	}

	if (state == PARSE_OFF)            return PARSE_NUL;
	if (line[0] == '\n')               return PARSE_BR;
	if (line[0] == '>')                return PARSE_Q;
	if (strncmp(line, "# ",   2) == 0) return PARSE_H1;
	if (strncmp(line, "## ",  3) == 0) return PARSE_H2;
	if (strncmp(line, "### ", 4) == 0) return PARSE_H3;
	if (strncmp(line, "=> ",  3) == 0) return PARSE_URI;
	if (strncmp(line, "* ",   2) == 0) return PARSE_LI;
	if (strncmp(line, "```",  3) == 0) return PARSE_OFF;

	return PARSE_P;
}
