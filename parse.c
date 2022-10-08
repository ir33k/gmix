#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "str.h"
#include "util.h"

void
parse_pstate(Parse state, FILE *fp)
{
	if (state & PARSE_NUL)  fprintf(fp, "NUL\t");
	if (state & PARSE_RES)  fprintf(fp, "RES\t");
	if (state & PARSE_H1)   fprintf(fp, "H1\t");
	if (state & PARSE_H2)   fprintf(fp, "h2\t");
	if (state & PARSE_H3)   fprintf(fp, "h3\t");
	if (state & PARSE_P)    fprintf(fp, "P\t");
	if (state & PARSE_BR)   fprintf(fp, "BR\t");
	if (state & PARSE_URI)  fprintf(fp, "URI\t");
	if (state & PARSE_LI)   fprintf(fp, "LI\t");
	if (state & PARSE_Q)    fprintf(fp, "Q\t");
	if (state & PARSE_PRE)  fprintf(fp, "PRE\t");
	if (state & PARSE_URIL) fprintf(fp, "URIL\t");
	if (state & PARSE_URID) fprintf(fp, "URID\t");
	if (state & PARSE_BEG)  fprintf(fp, "BEG\t");
	if (state & PARSE_END)  fprintf(fp, "END\t");

	fprintf(fp, "\b\n");
}

Parse
parse(Parse state, char *line, size_t siz, FILE *fp)
{
	Parse    res;		/* Result, new parse state */
	size_t   len;

	if (fgets(line, siz, fp) == NULL)
		return PARSE_NUL;

	/* TODO(irek): PARSE_ERR could indicate fail in fgets. */

	res = 0;

	/* Check if previous state was had line end.  If yes then this
	 * should be a new line beginning, else use previous state
	 * without beginning and ending flags. */
	if (state == PARSE_NUL || (state & PARSE_END)) {
		res = PARSE_BEG;
	} else {
		res = state;
		if (res & PARSE_BEG) res ^= PARSE_BEG;
		if (res & PARSE_END) res ^= PARSE_END;
	}

	len = strlen(line);

	if (line[len-1] == '\n')
		res |= PARSE_END;

	if (state & PARSE_PRE) {
		if ((res & PARSE_BEG) && strncmp(line, "```", 3) == 0)
			return res | PARSE_BR;

		return res | PARSE_PRE;
	}

	if (!(res & PARSE_BEG))
		return res;    /* End here if not beginning of line */

	/* TODO(irek): Missing PARSE_RES.  I'm not sure how to detect
	 * header line properly.  For sure it could be only in first
	 * line so when given STATE is PARSE_NUL but relaying on
	 * header line starting with 2 digit number is to risky.  Any
	 * first line on capsule page could start with 2 digit number.
	 * There must be a better way.  Anyway this validation of
	 * response header should will be implemented in res.h lib. */

	/* Most of lines starts with some character and whitespace
	 * which is any number of spaces or tabs.  To make code simple
	 * I just duplicated those to check first for space then for
	 * tab \t. */

	/**/ if (len == 1)                       res |= PARSE_BR;
	else if (strncmp(line, ">",     1) == 0) res |= PARSE_Q;
	else if (strncmp(line, "# ",    2) == 0) res |= PARSE_H1;
	else if (strncmp(line, "#\t",   2) == 0) res |= PARSE_H1;
	else if (strncmp(line, "## ",   3) == 0) res |= PARSE_H2;
	else if (strncmp(line, "##\t",  3) == 0) res |= PARSE_H2;
	else if (strncmp(line, "### ",  4) == 0) res |= PARSE_H3;
	else if (strncmp(line, "###\t", 4) == 0) res |= PARSE_H3;
	else if (strncmp(line, "=>",    2) == 0) res |= PARSE_URI;
	else if (strncmp(line, "* ",    2) == 0) res |= PARSE_LI;
	else if (strncmp(line, "*\t",   2) == 0) res |= PARSE_LI;
	else if (strncmp(line, "```",   3) == 0) res |= PARSE_PRE;
	else                                     res |= PARSE_P;

	return res;
}

char *
parse_clean(Parse state, char *line)
{
	size_t   len;

	/* Trim leading line type markup characters. */
	if ((state & PARSE_BEG) &&
	    ((state & PARSE_Q) ||
	     (state & PARSE_H1) ||
	     (state & PARSE_H2) ||
	     (state & PARSE_H3) ||
	     (state & PARSE_URI) ||
	     (state & PARSE_LI) ||
	     (strncmp(line, "```",  3) == 0))) {
		/* Get to the first whitespace or \0. */
		while (line[0] != ' ' && line[0] != '\t' && line[0] != '\0')
			line += 1;

		line = str_triml(line);
	}

	/* Trim ending new line char. */
	if (state & PARSE_END) {
		len = strlen(line);

		/* This condition should always be true when PARSE_END
		 * flag is set but let's do it anyway. */
		if (line[len-1] == '\n')
			line[len-1] = '\0';
	}

	return line;
}

int
_parse_untilc(char *line, char end)
{
	size_t	i;

	for (i = 0; line[i] != '\0'; i++) {
		if (line[i] == end) {
			line[i] = '\0';
			return 1;
		}
	}

	return 0;
}
