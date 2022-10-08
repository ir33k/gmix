/* Parser for text/gemini */

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>

/*
 * Parse flags for state and Line Types.
 */
typedef enum {
	PARSE_NUL  = 0,		/* Start and end parsing state */
	/* LINE TYPE */
	PARSE_RES  = 1 << 0,	/* Response header */
	PARSE_H1   = 1 << 1,	/* Heading level 1 */
	PARSE_H2   = 1 << 2,	/* Heading level 2 */
	PARSE_H3   = 1 << 3,	/* Heading level 3 */
	PARSE_P    = 1 << 4,	/* Regular text (paragraph) */
	PARSE_BR   = 1 << 5,	/* New lines section */
	PARSE_URI  = 1 << 6,	/* For links lines */
	PARSE_LI   = 1 << 7,	/* Unordered list item */
	PARSE_Q    = 1 << 8,	/* Quote */
	PARSE_PRE  = 1 << 9,	/* Preformatted text, no formatting */
	/* URI SECTIONS */
	PARSE_URIL = 1 << 10,	/* URI URL link */
	PARSE_URID = 1 << 11,	/* URI description */
	/* LINE INFO */
	PARSE_BEG  = 1 << 12,	/* Beginning of line */
	PARSE_END  = 1 << 13	/* End of line */
} Parse;

/*
 * Prints to FP stream STATE in string format.  Usefull for debugging.
 */
void parse_pstate(Parse state, FILE *fp);

/*
 * Given previous parser STATE (0 on first usage) put bites from FP
 * file to LINE buffer of SIZ size making it null-terminated string.
 * Works like fgets but return PARSE state of current LINE content.
 * State will contain one of "LINE TYPE" flags and any number of "LINE
 * INFO" flags.  Return PARSE_NUL when parsing comes to the end.
 *
 * I believe that the smallest buffer LINE SIZ size possible is the
 * longest markup prefix + end line + null which gives 6.  I don't
 * know if there is any reason to go that low.  It's probably more
 * efficient to use default BUFSIZ value.
 */
Parse parse(Parse state, char *line, size_t siz, FILE *fp);

/*
 * Based on given parser STATE strip given STR string from leading
 * markup characters and trailing new line character.  Return pointer
 * to modified string.  Given STR string will be modified in the
 * process.  This function is useful because it delivers just the
 * content of parsed line.  New line character is removed because in
 * some output formats new line is controlled differently than by \n
 * like in HTML.  Note that preformatted lines should not be modified
 * so either don't use this function on those or put back new line
 * character for PARSE_PRE lines.
 */
char *parse_clean(Parse state, char *line);

/* TODO(irek): Do I need this? */
/*
 * Get content of STR string until reaching first encounter of END
 * character.  Function will place \0 in place of END character and
 * return 1.  If END is not fount then 0 is returned.
 */
int _parse_untilc(char *str, char end);

#endif	/* PARSE_H_ */
