/* Parse text/gemini text format to HTML syntax. */

#include <stdio.h>

#include "util.h"
#include "parse.h"

/* It's probably better to use default BUFSIZ value for buffer size
 * which on modern machines is much higher but that would result in
 * parsing each line as a whole and I wan't to do it in small parts to
 * test parser capabilities and catch mistakes. */

#define BSIZ    32		/* Buffer size */
#define INDENT  "\t"		/* Indentation for HTML tags */

/*
 * Print program usage instruction using provided progrm NAME.
 */
void usage(char *name);

/* TODO(irek): This program don't require any arguments so strategy to
 * print usage text when no argument is provided doesn't work here. */

int
main(int argc, char **argv)
{
	PARSE   old;		/* Previous parsing state */
	PARSE   new;		/* New parsing state */
	char    buf[BSIZ];	/* For reading file */
	FILE   *fp;		/* File Pointer */

	fp  = stdin;		/* Read from stdin by default */
	old = PARSE_NUL;

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	while ((new = parse(old, buf, BSIZ, fp)) != PARSE_NUL) {
		/* Note that PARSE_BR is ignored in HTML parser
		 * because new lines are achieved by paragraph,
		 * header, list and other tags by itself. */

		/* Open or close tag groups when old and new status
		 * differs between parsing iterations. */
		/**/ if (flagged(new, PARSE_URI) && !flagged(old, PARSE_URI))
			printf("<ul class=\"links\">\n");
		else if (flagged(new, PARSE_LI) && !flagged(old, PARSE_LI))
			printf("<ul>\n");
		else if (flagged(new, PARSE_PRE) && !flagged(old, PARSE_PRE))
			printf("<pre>\n");
		else if (flagged(old, PARSE_URI) && !flagged(new, PARSE_URI))
			printf("</ul>\n");
		else if (flagged(old, PARSE_LI) && !flagged(new, PARSE_LI))
			printf("</ul>\n");
		else if (flagged(old, PARSE_PRE) && !flagged(new, PARSE_PRE))
			printf("</pre>\n");

		/* Open tag. */
		if (flagged(new, PARSE_BEG)) {
			/**/ if (flagged(new, PARSE_H1))
				printf("<h1>");
			else if (flagged(new, PARSE_H2))
				printf("<h2>");
			else if (flagged(new, PARSE_H3))
				printf("<h3>");
			else if (flagged(new, PARSE_P))
				printf("<p>");
			else if (flagged(new, PARSE_URI))
				printf(INDENT "<li><a>");
			else if (flagged(new, PARSE_LI))
				printf(INDENT "<li>");
			else if (flagged(new, PARSE_Q))
				printf("<blockquote>");
			else if (flagged(new, PARSE_RES))
				printf("<head>");
		}

		/* Print tag content. */
		fputs(parse_clean(new, buf), stdout);

		/* Close tag. */
		if (flagged(new, PARSE_END)) {
			/* Add new lines for preformatted content. */
			if (flagged(new, PARSE_PRE))
				putchar('\n');

			/**/ if (flagged(new, PARSE_H1))
				printf("</h1>\n");
			else if (flagged(new, PARSE_H2))
				printf("</h2>\n");
			else if (flagged(new, PARSE_H3))
				printf("</h3>\n");
			else if (flagged(new, PARSE_P))
				printf("</p>\n");
			else if (flagged(new, PARSE_URI))
				printf("</a></li>\n");
			else if (flagged(new, PARSE_LI))
				printf("</li>\n");
			else if (flagged(new, PARSE_Q))
				printf("</blockquote>\n");
			else if (flagged(new, PARSE_RES))
				printf("</head>\n");
		}

		/* Store last parsing status. */
		old = new;
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
	printf("(GMI 2 HTML) Parse Gemeni text to HTML.\n\n"
	       "\tusage: %s [file]\n\n",
	       name);
}
