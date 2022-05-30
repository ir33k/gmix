/* Parser for text/gemini */

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>

/*
 * Parse flags for state and Line Types
 */
typedef enum {
	PARSE_NUL = 0,		/* Init and end parsing state */
	PARSE_H1  = 1 << 0,	/* Heading level 1 */
	PARSE_H2  = 1 << 1,	/* Heading level 2 */
	PARSE_H3  = 1 << 2,	/* Heading level 3 */
	PARSE_P   = 1 << 3,	/* Regular text (paragraph) */
	PARSE_BR  = 1 << 4,	/* New lines section */
	PARSE_URI = 1 << 5,	/* For links lines */
	PARSE_LI  = 1 << 6,	/* Unordered list item */
	PARSE_Q   = 1 << 7,	/* Quote */
	PARSE_PRE = 1 << 8,	/* Preformatted text, no formatting */
	PARSE_RES = 1 << 9,	/* Response header */
	PARSE_BEG = 1 << 10,	/* Beginning of line */
	PARSE_END = 1 << 11	/* End of line */
} PARSE;

char *parse_clean(PARSE state, char *str);

/*
 * Given previous parser STATE (0 on first usage) put bites from FP
 * file to BUF buffer of SIZ size.  Works like fgets but return PARSE
 * state of current BUF content.
 */
PARSE parse(PARSE state, char *line, size_t siz, FILE *fp);

#endif	/* PARSE_H_ */
