/* Parser tool for text/gemini */

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>

/* Parse state and Line Types */
typedef enum {
	PARSE_ERR = -1,		/* Parse error */
	PARSE_BEG = 0,		/* Beginning of parsing */
	PARSE_WIP,		/* In the middle of the line */
	PARSE_RES,		/* Response header */
	PARSE_H1,		/* Heading level 1 */
	PARSE_H2,		/* Heading level 2 */
	PARSE_H3,		/* Heading level 3 */
	PARSE_P,		/* Regular text (paragraph) */
	PARSE_BR,		/* New lines section */
	PARSE_URI,		/* For links lines */
	PARSE_LI,		/* Unordered list item */
	PARSE_Q,		/* Quote */
	PARSE_OFF,		/* Disable formatting */
	PARSE_NUL,		/* No formatting */
	PARSE_ON,		/* Enable formatting */
	PARSE_END		/* End of parsing */
} PARSE;

PARSE parse(PARSE state, char *buf, size_t siz, FILE *fp);

#endif	/* PARSE_H_ */
