#include <stdio.h>

#include "util.h"
#include "parse.h"

#define BSIZ	256		/* Buffer size */

/*
 * Print program usage instruction using provided progrm NAME.
 */
void usage(char *name);

/* TODO(irek): This program don't require any arguments so strategy to
 * print usage text when no argument is provided doesn't work here. */

int
main(int argc, char **argv)
{
	PARSE   state;		/* Parsing state */
	char    buf[BSIZ];	/* For reading file */
	FILE   *fp;

	state = PARSE_NUL;
	fp  = stdin;		/* Read from stdin by default */

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	while ((state = parse(state, buf, BSIZ, fp)) != PARSE_NUL) {
		if (flagged(state, PARSE_BEG)) {
			/**/ if (flagged(state, PARSE_H1))  printf("h1");
			else if (flagged(state, PARSE_H2))  printf("h2");
			else if (flagged(state, PARSE_H3))  printf("h3");
			else if (flagged(state, PARSE_P))   printf("p");
			else if (flagged(state, PARSE_BR))  printf("br");
			else if (flagged(state, PARSE_URI)) printf("uri");
			else if (flagged(state, PARSE_LI))  printf("li");
			else if (flagged(state, PARSE_Q))   printf("quote");
			else if (flagged(state, PARSE_PRE)) printf("pre");
			else if (flagged(state, PARSE_RES)) printf("res");

			putchar('\t');
		}

		fputs(parse_clean(state, buf), stdout);

		if (flagged(state, PARSE_END))
			putchar('\n');
	}

	if (fp != stdin && fclose(fp) == EOF)
		die("fclose:");

	return 0;
}

void
usage(char *name)
{
	/* TODO(irek): Function is not used yet and usage instruction
	 * itself is also not correct. */
	printf("(GMI Parser) Parse Gemeni text.\n\n"
	       "\tusage: %s [file]\n\n",
	       name);
}
