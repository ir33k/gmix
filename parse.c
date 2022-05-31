#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "util.h"

char *
parse_clean(PARSE state, char *str)
{
	size_t   len;

	/* Trim leading line type markup characters. */
	if (flagged(state, PARSE_BEG) &&
	    (flagged(state, PARSE_Q) ||
	     flagged(state, PARSE_H1) ||
	     flagged(state, PARSE_H2) ||
	     flagged(state, PARSE_H3) ||
	     flagged(state, PARSE_URI) ||
	     flagged(state, PARSE_LI) ||
	     (strncmp(str, "```",  3) == 0))) {
		while (str[0] != ' ' && str[0] != '\t' && str[0] != '\0')
			str += 1;

		while (str[0] == ' ' || str[0] == '\t')
			str += 1;
	}

	/* Trim ending new line char. */
	if (flagged(state, PARSE_END)) {
		len = strlen(str);

		/* This condition should always be true when PARSE_END
		 * flag is set but let's do it anyway. */
		if (str[len-1] == '\n')
			str[len-1] = '\0';
	}

	return str;
}

PARSE
parse(PARSE state, char *line, size_t siz, FILE *fp)
{
	PARSE    res;		/* Result, new parse state */
	size_t   len;

	if (fgets(line, siz, fp) == NULL)
		return PARSE_NUL;

	/* TODO(irek): PARSE_ERR could indicate fail in fgets. */

	res = 0;

	/* Check if previous state was had line end.  If yes then this
	 * should be a new line beginning, else use previous state
	 * without beginning and ending flags. */
	if (state == PARSE_NUL || flagged(state, PARSE_END)) {
		res = PARSE_BEG;
	} else {
		res = state;
		if (flagged(res, PARSE_BEG)) res ^= PARSE_BEG;
		if (flagged(res, PARSE_END)) res ^= PARSE_END;
	}

	len = strlen(line);

	if (line[len-1] == '\n')
		res |= PARSE_END;

	if (flagged(state, PARSE_PRE)) {
		if (flagged(res, PARSE_BEG) && strncmp(line, "```", 3) == 0)
			return res | PARSE_BR;

		return res | PARSE_PRE;
	}

	if (flagged(res, PARSE_BEG) == 0)
		return res;

	/* TODO(irek): Missing PARSE_RES.  I'm not sure how to detect
	 * header line properly.  For sure it could be only in first
	 * line so when given STATE is PARSE_NUL but relaying on
	 * header line starting with 2 digit number is to risky.  Any
	 * first line on capsule page could start with 2 digit number.
	 * There must be a better way. */

	/**/ if (len == 1)                      res |= PARSE_BR;
	else if (strncmp(line, "> ",   2) == 0) res |= PARSE_Q;
	else if (strncmp(line, "# ",   2) == 0) res |= PARSE_H1;
	else if (strncmp(line, "## ",  3) == 0) res |= PARSE_H2;
	else if (strncmp(line, "### ", 4) == 0) res |= PARSE_H3;
	else if (strncmp(line, "=> ",  3) == 0) res |= PARSE_URI;
	else if (strncmp(line, "* ",   2) == 0) res |= PARSE_LI;
	else if (strncmp(line, "```",  3) == 0) res |= PARSE_PRE;
	else                                    res |= PARSE_P;

	return res;
}
