/* Parse text/gemini text format to HTML syntax. */

#include <stdio.h>
#include <getopt.h>

#include "util.h"
#include "parse.h"

/* It's probably better to use default BUFSIZ value defined in stdio.h
 * for buffer size which on modern machines is much higher but that
 * would result in parsing each line as a whole and I want to do it in
 * small parts to test parser capabilities and catch mistakes. */

#define BSIZ    32		/* Buffer size */
#define INDENT  "\t"		/* Indentation for HTML tags */

int
main(int argc, char **argv)
{
	/* 2 parse states, OLD used to keep previous parse state and
	 * NEW for currently parsed part.  This allows to easily
	 * detect when parsing block of the same lines like links or
	 * list items lines started and ended.  LINE is main parser
	 * buffer and FP is parsed stream that defaults to stdin. */
	Parse old, new;
	char line[BSIZ];
	FILE *fp;

	if (getopt(argc, argv, "h") != -1) {
		printf("GMI 2 HTML - Parse Gemeni text to HTML.\n\n"
		       "usage: %s [-h] [file]\n\n"
		       "\t-h\tPrint this usage help message.\n"
		       "\tfile\tFile to parse, use stdin by default.\n\n",
		       argv[0]);
		return 1;
	}

	fp  = stdin;		/* Read from stdin by default */
	old = PARSE_NUL;

	/* TODO(irek): Make it possible to parse multiple files? */

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	while ((new = parse(old, line, BSIZ, fp)) != PARSE_NUL) {
		/* Note that PARSE_BR is ignored in HTML parser
		 * because new lines are achieved by opening and
		 * closing block tags. */

		/* Open or close tag groups when old and new status
		 * differs between parsing iterations. */
		/**/ if ((new & PARSE_URI) && !(old & PARSE_URI))
			printf("<ul class=\"links\">\n");
		else if ((new & PARSE_LI)  && !(old & PARSE_LI))
			printf("<ul>\n");
		else if ((new & PARSE_PRE) && !(old & PARSE_PRE))
			printf("<pre>\n");
		else if ((old & PARSE_URI) && !(new & PARSE_URI))
			printf("</ul>\n");
		else if ((old & PARSE_LI)  && !(new & PARSE_LI))
			printf("</ul>\n");
		else if ((old & PARSE_PRE) && !(new & PARSE_PRE))
			printf("</pre>\n");

		/* Open tag. */
		if (new & PARSE_BEG) {
			/**/ if (new & PARSE_H1)  printf("<h1>");
			else if (new & PARSE_H2)  printf("<h2>");
			else if (new & PARSE_H3)  printf("<h3>");
			else if (new & PARSE_P)   printf("<p>");
			else if (new & PARSE_URI) printf(INDENT "<li><a>");
			else if (new & PARSE_LI)  printf(INDENT "<li>");
			else if (new & PARSE_Q)   printf("<blockquote>");
			else if (new & PARSE_RES) printf("<head>");
		}

		/* Print tag content. */
		fputs(parse_clean(new, line), stdout);

		/* Close tag. */
		if (new & PARSE_END) {
			/**/ if (new & PARSE_PRE) putchar('\n');
			else if (new & PARSE_H1)  printf("</h1>\n");
			else if (new & PARSE_H2)  printf("</h2>\n");
			else if (new & PARSE_H3)  printf("</h3>\n");
			else if (new & PARSE_P)   printf("</p>\n");
			else if (new & PARSE_URI) printf("</a></li>\n");
			else if (new & PARSE_LI)  printf("</li>\n");
			else if (new & PARSE_Q)   printf("</blockquote>\n");
			else if (new & PARSE_RES) printf("</head>\n");
		}

		old = new;	/* Store last parsing status */
	}

	if (fp != stdin && fclose(fp) == EOF)
		die("fclose:");

	return 0;
}
