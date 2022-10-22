/* Parse text/gemini text format to HTML syntax. */

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "util.h"
#include "parse.h"

/* It's probably better to use default BUFSIZ value defined in stdio.h
 * for buffer size which on modern machines is much higher but that
 * would result in parsing each line as a whole and I want to do it in
 * small parts to test parser capabilities and catch mistakes. */

#define BSIZ    32		/* Buffer size */
#define URLSIZ  1024+1		/* Buffer size that can hold URL */
#define INDENT  "\t"		/* Indentation for HTML tags */

/* If VAL is smaller than MIN, return MIN.  When it's bigger than MAX,
 * return MAX.  Otherwise return original VAL.*/
int clamp(int min, int val, int max);

int
clamp(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

int
main(int argc, char **argv)
{
	/* 2 parse states, OLD used to keep previous parse state and
	 * NEW for currently parsed part.  This allows to easily
	 * detect when parsing block of the same lines like links or
	 * list items lines started and ended.  URL is for holding
	 * whole last parsed url so it's possible to print it later
	 * again when optional url desciption is not provided.  BUF is
	 * main parser buffer and FP is parsed stream that defaults to
	 * stdin. */
	Parse old = PARSE_NUL, new;
	char url[URLSIZ];
	char buf[BSIZ];
	FILE *fp = stdin;	/* Read from stdin by default */

	if (getopt(argc, argv, "h") != -1) {
		printf("GMI 2 HTML - Parse Gemeni text to HTML.\n\n"
		       "usage: %s [-h] [file]\n\n"
		       "\t-h\tPrint this usage help message.\n"
		       "\tfile\tFile to parse, use stdin by default.\n\n",
		       argv[0]);
		return 1;
	}

	/* TODO(irek): Make it possible to parse multiple files? */

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	while ((new = parse(old, buf, BSIZ, fp))) {
		/* Note that PARSE_BR is ignored in HTML parser
		 * because new lines are achieved by opening and
		 * closing block tags. */

		/* TODO(irek): Working with blocks and links optional
		 * desciptions is a bit complicated.  In case of links
		 * it might be simpler to always return DSC after URL
		 * but it might be empty string.  IDK what to do with
		 * block tho. */

		/* Open or close tag groups when old and new status
		 * differs between parsing iterations. */
		/**/ if ((new & PARSE_URL) && !(old & PARSE_URL) && !(old & PARSE_DSC))
			printf("<ul class=\"links\">\n");
		else if ((new & PARSE_ULI) && !(old & PARSE_ULI))
			printf("<ul>\n");
		else if ((new & PARSE_PRE) && !(old & PARSE_PRE))
			printf("<pre>\n");
		else if ((new & PARSE_URL) && (old & PARSE_URL) && (old & PARSE_END) && !(new & PARSE_DSC))
			printf("%s</a></li>\n", url);
		else if ((old & PARSE_URL) && !(new & PARSE_URL) && !(new & PARSE_DSC))
			printf("%s</a></li>\n</ul>\n", url);
		else if ((old & PARSE_DSC) && (old & PARSE_END) && !(new & PARSE_URL))
			printf("</ul>\n");
		else if ((old & PARSE_URL) && !(new & PARSE_URL) && !(new & PARSE_DSC))
			printf("</ul>\n");
		else if ((old & PARSE_ULI) && !(new & PARSE_ULI))
			printf("</ul>\n");
		else if ((old & PARSE_PRE) && !(new & PARSE_PRE))
			printf("</pre>\n");

		/* Open tag. */
		if (new & PARSE_BEG) {
			/**/ if (new & PARSE_H1)  printf("<h1>");
			else if (new & PARSE_H2)  printf("<h2>");
			else if (new & PARSE_H3)  printf("<h3>");
			else if (new & PARSE_P)   printf("<p>");
			else if (new & PARSE_URL) printf(INDENT "<li><a href=\"");
			else if (new & PARSE_DSC); /* Do nothing */
			else if (new & PARSE_ULI) printf(INDENT "<li>");
			else if (new & PARSE_Q)   printf("<blockquote>");
		}

		/* Remember last URL so it's possible to print it
		 * again if optional DSC for link is not provided. */
		if (new & PARSE_URL) {
			if (new & PARSE_BEG) {
				url[0] = '\0';
			}
			strncat(url, buf, clamp(0, strlen(buf), URLSIZ - strlen(url) - 1));
		}
		
		/* Print content of buf. */
		fputs(buf, stdout);

		/* Close tag. */
		if (new & PARSE_END) {
			/**/ if (new & PARSE_PRE) putchar('\n');
			else if (new & PARSE_H1)  printf("</h1>\n");
			else if (new & PARSE_H2)  printf("</h2>\n");
			else if (new & PARSE_H3)  printf("</h3>\n");
			else if (new & PARSE_P)   printf("</p>\n");
			else if (new & PARSE_URL) printf("\">");
			else if (new & PARSE_DSC) printf("</a></li>\n");
			else if (new & PARSE_ULI) printf("</li>\n");
			else if (new & PARSE_Q)   printf("</blockquote>\n");
		}

		old = new;	/* Store last parsing status */
	}

	if (fp != stdin && fclose(fp) == EOF)
		die("fclose:");

	return 0;
}
