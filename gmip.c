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
	int     eol;		/* 0 when EOL not reached */
	char    buf[BSIZ];	/* For reading file */
	LT	lt;		/* Line Type */
	LT	prev;		/* Previous Line Type */
	FILE   *fp;

	eol = 1;		/* Indicate fresh line */
	lt  = LT_P;		/* Init with 0 */
	fp  = stdin;		/* Read from stdin by default */

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	/* TODO(irek): Skip first line ignoring response header for
	 * now and focuse on parsing text/gemini content. */
	while (fgets(buf, BSIZ, fp) != NULL && !parse__eol(buf));

	while (fgets(buf, BSIZ, fp) != NULL) {
		/* TODO(irek): Any idea how to do it elegantly without
		 * goto?  If so then tell me plz. */
		if (eol == 0)
			goto just_print;

		prev = lt;
		lt = parse__lt(prev, buf);
		
		switch (lt) {
		case LT_P:
			printf("p");
			break;
		case LT_H1:
			printf("h1");
			break;
		case LT_H2:
			printf("h2");
			break;
		case LT_H3:
			printf("h3");
			break;
		case LT_BR:
			printf(prev == LT_PRE ? ")": "br");
			break;
		case LT_URI:
			printf("url");
			break;
		case LT_LI:
			printf("li");
			break;
		case LT_Q:
			printf("quote");
			break;
		case LT_PRE:
			printf(prev == LT_PRE ? "" : "(");
			break;
		case LT_RES:
			printf("header");
			break;
		}
		putchar('\t');
	just_print:
		fputs(buf, stdout);
		/* TODO(irek): Parser functions are still used with __
		 * private notation because I wasn't planning on using
		 * them outside lib at all.  I have to rethink it. */
		eol = parse__eol(buf);
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
