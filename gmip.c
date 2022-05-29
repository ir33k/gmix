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

	state = PARSE_BEG;
	fp  = stdin;		/* Read from stdin by default */

	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}

	while ((state = parse(state, buf, BSIZ, fp)) != PARSE_END) {
		switch (state) {
		case PARSE_END:
			die("Parse should be ended by while loop");
			break;
		case PARSE_ERR:
			die("Parse returned error");
			break;
		case PARSE_BEG:
			die("Parse should not return PARSE_BEG");
			break;
		case PARSE_WIP:
			goto just_print;
		case PARSE_RES:
			printf("res");
			break;
		case PARSE_H1:
			printf("h1");
			break;
		case PARSE_H2:
			printf("h2");
			break;
		case PARSE_H3:
			printf("h3");
			break;
		case PARSE_P:
			printf("p");
			break;
		case PARSE_BR:
			printf("br");
			break;
		case PARSE_URI:
			printf("url");
			break;
		case PARSE_LI:
			printf("li");
			break;
		case PARSE_Q:
			printf("quote");
			break;
		case PARSE_OFF:
			printf("off");
			break;
		case PARSE_NUL:
			break;
		case PARSE_ON:
			printf("on");
			break;
		}
		putchar('\t');
	just_print:
		fputs(buf, stdout);
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
