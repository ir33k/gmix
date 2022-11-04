/* Parse text/gemini text format. */

#include <stdio.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIP_IMPLEMENTATION
#include "gmip.h"

/* Print program instruction with ARGV0 program name, exit with 1. */
void usage(char *argv0);

void
usage(char *argv0)
{
	die("Parse text/gemini format to normalized Gemini, simplest\n"
	    "stadnard output with prefixes, Markdown or HTML.\n\n"
	    "usage: %s [-h] [-f (gmi|std|md|html)] [file]\n\n"
	    "\t-f\tParse to one of supported formats, default to std.\n"
	    "\t-h\tPrint this usage help message.\n"
	    "\tfile\tFile to parse, use stdin by default.\n",
	    argv0);
}

int
main(int argc, char **argv)
{
	struct gmip ps = {0};
	char str[BUFSIZ], opt;
	FILE *fp = stdin;
	/* PARSE points to one of gmip.h parse functions.  This is
	 * possible because all gmip.h parse functions uses the same
	 * arguments.  Default to gmip_2std. */
	int (*parse)(struct gmip*, char*, size_t, FILE*) = &gmip_2std;

	while ((opt = getopt(argc, argv, "f:h")) != -1) {
		switch (opt) {
		case 'f':
			/**/ if (!strcmp(optarg, "gmi"))  parse = &gmip_2gmi;
			else if (!strcmp(optarg, "std"))  parse = &gmip_2std;
			else if (!strcmp(optarg, "md"))   parse = &gmip_2md;
			else if (!strcmp(optarg, "html")) parse = &gmip_2html;
			else usage(argv[0]);
			break;
		case 'h':
		default:
			usage(argv[0]);
		}
	}
	if (argc - optind && (fp = fopen(argv[argc-1], "rb")) == NULL) {
		die("fopen:");
	}
	while (parse(&ps, str, BUFSIZ, fp)) {
		fputs(str, stdout);
	}
	if (fp != stdin && fclose(fp) == EOF) {
		die("fclose:");
	}
	return 0;
}
