/* Parse text/gemini text format so lines are prefixed with type. */

#include <stdio.h>
#include <getopt.h>

#include "util.h"
#include "parse.h"

/* Small buffer size is used to test how well parse.h deals with lines
 * delivered in multiple parts. */

#define BSIZ	8		/* Buffer size */

int
main(int argc, char **argv)
{
	Parse state;
	char line[BSIZ];
	FILE *fp;

	if (getopt(argc, argv, "h") != -1) {
		printf("GMI Parser - Parse Gemeni text.\n\n"
		       "usage: %s [-h] [file]\n\n"
		       "\t-h\tPrint this usage help message.\n"
		       "\tfile\tFile to parse, use stdin by default.\n\n",
		       argv[0]);
		return 1;
	}

	state = PARSE_NUL;
	fp  = stdin;		/* Read from stdin by default */

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	while ((state = parse(state, line, BSIZ, fp)) != PARSE_NUL) {
		/* Print prefix for each line type. */
		if (state & PARSE_BEG) {
			/**/ if (state & PARSE_H1)  printf("h1");
			else if (state & PARSE_H2)  printf("h2");
			else if (state & PARSE_H3)  printf("h3");
			else if (state & PARSE_P)   printf("p");
			else if (state & PARSE_BR)  printf("br");
			else if (state & PARSE_URI) printf("uri");
			else if (state & PARSE_LI)  printf("li");
			else if (state & PARSE_Q)   printf("quote");
			else if (state & PARSE_PRE) printf("pre");
			else if (state & PARSE_RES) printf("res");
			putchar('\t');
		}

		/* Handle URI with dedicated functions. */
		/* if (state & PARSE_URI) { */
		/* 	#error "WIP: parse URI url and message." */
		/* } */

		/* Print content of line. */
		fputs(parse_clean(state, line), stdout);

		/* End line. */
		if (state & PARSE_END)
			putchar('\n');
	}

	if (fp != stdin && fclose(fp) == EOF)
		die("fclose:");

	return 0;
}
