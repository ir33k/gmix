/* GMI Parser for text/gemini.  Allows to easily go through stream
 * filling given buffer peace by peace along with struct gmip that
 * will tell what kind of line is it and at which part of line parsing
 * we are.  For simplest example see gmip2txt.c program. */

#ifndef GMIP_H_
#define GMIP_H_

#include <stdio.h>

/* Line types.  NUL should never be returned.  It's used internally by
 * gmip function.  ALL line types ends with '\n' except for URL and
 * PRE.  First can end with '\n' but can also end with ' ' or '\t'.
 * DSC is always returned as next type after URL.  DSC is optional so
 * if it's not provided STR (from gmip function) will be empty.  PRE
 * ends with "```" and it's the only line type that will include new
 * line characters and whitespaces in STR. */
enum gmip_lt {
	GMIP_NUL = 0,		/* Empty type */
	GMIP_H1,		/* Heading level 1 */
	GMIP_H2,		/* Heading level 2 */
	GMIP_H3,		/* Heading level 3 */
	GMIP_P,			/* Regular text, paragraph, default */
	GMIP_LI,		/* Unordered list item */
	GMIP_Q,			/* Quote */
	GMIP_URL,		/* URL link */
	GMIP_DSC,		/* URL description */
	GMIP_PRE		/* Preformatted text, no formatting */
};

/* Parse State.  All values are read only.  Initialize struct with 0.
 * OLD and NEW are previous and current Line Type.  BEG is non 0 when
 * NEW line just started and EOL is last line end character which is
 * non 0 when end of line of current NEW type was reached. */
struct gmip {
	enum gmip_lt old, new;
	int beg;
	char eol;
};

/* Forward position in FP stream until first character not mentioned
 * in SKIP string is reached.  Return that character, might be EOF. */
int gmip__fskip(FILE *fp, char *skip);

/* Get current line type from FP stream.  Function expects that
 * current stream position is at the beginning of file, end of line or
 * beginning of new line.  STR string has to be at least 5 bytes long
 * and will contain loaded characters, that is leading line prefix or
 * first few characters of paragraph.  DSC is never returned because
 * this line type is forced in gmip function always after URL. */
enum gmip_lt gmip__lt(char *str, FILE *fp);

/* Main parsing function.  With 0 initialized PS parser state put as
 * much bytes into STR string of SIZ size from FP stream.  Each time
 * function is run values in PS are updated to reflect state of
 * content in STR.  That includes OLD last line type, NEW current line
 * type, non 0 value in BEG if STR contains first line bytes and EOL
 * end of line character if end of parsed line was reached.  Returns
 * non 0 value as long as something was parsed. */
int gmip(struct gmip *ps, char *str, size_t siz, FILE *fp);

#endif	/* GMIP_H_ */
