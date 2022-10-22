/* Parser for text/gemini */

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>

typedef enum {
	/* PARSE STATE */
	PARSE_NUL = 0,		/* Init empty state */
	PARSE_EOF = 1 << 0,	/* End Of File, end of parsing */
	/* LINE KIND */
	PARSE_H1  = 1 << 1,	/* Heading level 1 */
	PARSE_H2  = 1 << 2,	/* Heading level 2 */
	PARSE_H3  = 1 << 3,	/* Heading level 3 */
	PARSE_P   = 1 << 4,	/* Regular text, paragraph */
	PARSE_URL = 1 << 5,	/* URL link */
	PARSE_DSC = 1 << 6,	/* URL description */
	PARSE_ULI = 1 << 7,	/* Unordered list item */
	PARSE_Q   = 1 << 8,	/* Quote */
	PARSE_PRE = 1 << 9,	/* Preformatted text, no formatting */
	/* LINE INFO */
	PARSE_BEG = 1 << 10,	/* Beginning of line */
	PARSE_END = 1 << 11	/* End of line */
} Parse;

/*
 * Advance position in FP stream untill current character is not one
 * of SKIP string characters or we reach EOF.  Return result of fseek.
 */
int parse__fskip(FILE *fp, char *skip);

/*
 * Based on OLD prase state return current kind of line in FP.
 */
Parse parse__kind(Parse old, FILE *fp);

/*
 * Based on OLD parse state (use value of PARSE_NUL on first call) put
 * to STR string as much as SIZ-1 (nul terminator) characters from FP
 * stream.  Return parse state informing what kind of line this is, is
 * it start (PARSE_BEG) or end (PARSE_END) of line.  In case when
 * state lack both flags the middle of line was parsed.  Return state
 * with PARSE_EOF flag when end of FP stream was reached.
 */
Parse parse(Parse old, char *str, size_t siz, FILE *fp);

#endif	/* PARSE_H_ */
