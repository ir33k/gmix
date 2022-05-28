/* Parser tool for text/gemini */

#ifndef PARSE_H_
#define PARSE_H_

/* Line Types */
typedef enum {
	LT_P = 0,		/* Regular text (paragraph) */
	LT_H1,			/* Heading level 1 */
	LT_H2,			/* Heading level 2 */
	LT_H3,			/* Heading level 3 */
	LT_BR,			/* New lines section */
	LT_URI,			/* For links lines */
	LT_LI,			/* Unordered list item */
	LT_Q,			/* Quote */
	LT_PRE,			/* Preformatted text */
	LT_RES			/* Response header */
} LT;

/*
 * Check whether given STR string line ends with new line char.
 */
int parse__eol(char *str);

/*
 * Take PREV previous Line Type and BUF buffer with at least first 4
 * line characters and determinate new Line Type.
 */
LT parse__lt(LT prev, char *buf);

#endif	/* PARSE_H_ */
