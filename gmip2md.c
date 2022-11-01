/* Parse text/gemini text format to Markdown syntax. */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIP_IMPLEMENTATION
#include "gmip.h"

int
main(int argc, char **argv)
{
	struct gmip ps = {0};
	char str[BUFSIZ];
	FILE *fp = stdin;

	if (getopt(argc, argv, "h") != -1) {
		die("Parse Gemeni text to Markdown.\n\n"
		    "usage: %s [-h] [file]\n\n"
		    "\t-h\tPrint this usage help message.\n"
		    "\tfile\tFile to parse, use stdin by default.\n",
		    argv[0]);
	}
	if (argc > 1 && (fp = fopen(argv[1], "rb")) == NULL) {
		die("fopen:");
	}
	while (gmip_2md(&ps, str, BUFSIZ, fp)) {
		fputs(str, stdout);
	}
	if (fp != stdin && fclose(fp) == EOF) {
		die("fclose:");
	}
	return 0;
}
