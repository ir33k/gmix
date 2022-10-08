/* Gemini URI parser - parse URL and print all URI parts. */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "util.h"
#include "uri.h"

/*
 * Print program usage instruction.  ARGV0 is program name.
 */
void usage(char *argv0);

void
usage(char *argv0)
{
	die("GMI URI - Parse Gemeni URL.\n\n"
	    "usage: %s [-fh] uri\n\n"
	    "\t-f\tPrint parsed URI in format compatible with gmif.\n"
	    "\t-h\tPrint this help usage message.\n"
	    "\turi\tURL string to parse.\n",
	    argv0);
}

int
main(int argc, char **argv)
{
	/* OPTF is to indicate if program was run with -f flag. */
	Uri uri;
	char opt;
	int optf = 0;

	if (argc < 2)
		usage(argv[0]);

	while ((opt = getopt(argc, argv, "fh")) != -1) {
		switch (opt) {
		case 'f':
			optf = 1;
			break;
		case 'h':
		default:
			usage(argv[0]);
		}
	}

	switch (uri_parse(&uri, argv[argc-1])) {
	case URI_PARSE_OK: break;
	case URI_PARSE_TOO_LONG: die("Parsed URI is too long");
	}

	/* Print output for gmif program if -f option was provided. */
	if (optf) {
		printf("%s %s %s",
		       uri.url,
		       uri.host ? uri.host : "",
		       uri.port ? uri.port : "");
		return 0;	/* End here */
	}

	/* Else print everything. */
	printf("%s\n", uri.url);
	printf("%s\n", uri.prot ? uri.prot : "");
	printf("%s\n", uri.host ? uri.host : "");
	printf("%s\n", uri.port ? uri.port : "");
	printf("%s\n", uri.path ? uri.path : "");
	printf("%s\n", uri.qstr ? uri.qstr : "");

	return 0;
}
