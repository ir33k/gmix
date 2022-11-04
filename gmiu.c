/* Gemini URI parser - parse URL and print all URI parts. */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIU_IMPLEMENTATION
#include "gmiu.h"

/* Print program usage instruction.  ARGV0 is program name. */
void usage(char *argv0);

void
usage(char *argv0)
{
	die("Parse Gemeni URI.\n\n"
	    "usage: %s [-vfh] uri\n\n"
	    "\t-v\tVerbose output describing each URI part.\n"
	    "\t-f\tPrint parsed URI in format compatible with gmif.\n"
	    "\t-h\tPrint this help usage message.\n"
	    "\turi\tURL string to parse.\n",
	    argv0);
}

int
main(int argc, char **argv)
{
	/* OPT_V and OPT_F are used to indicate if program was run
	 * with V and F flags. */
	int opt_v = 0, opt_f = 0;
	char opt;
	struct gmiu_uri uri;

	if (argc < 2) {
		usage(argv[0]);
	}
	while ((opt = getopt(argc, argv, "vfh")) != -1) {
		switch (opt) {
		case 'v': opt_v = 1; break;
		case 'f': opt_f = 1; break;
		case 'h':
		default:
			usage(argv[0]);
		}
	}
	switch (gmiu_parse(&uri, argv[argc-1])) {
	case GMIU_PARSE_OK: break;
	case GMIU_PARSE_TOO_LONG: die("Parsed URI is too long");
	}
	/* Print verbouse output. */
	if (opt_v) {
		printf("Normalized:\t%s\n", uri.url);
		printf("Protocol:\t%s\n", uri.prot ? uri.prot : "");
		printf("Host name:\t%s\n", uri.host ? uri.host : "");
		printf("Port number:\t%s\n", uri.port ? uri.port : "");
		printf("Resource path:\t%s\n", uri.path ? uri.path : "");
		printf("Query string:\t%s\n", uri.qstr ? uri.qstr : "");
		return 0;
	}
	/* Print output for gmif program if -f option was provided. */
	if (opt_f) {
		printf("%s %s %s",
		       uri.url,
		       uri.host ? uri.host : "",
		       uri.port ? uri.port : "");
		return 0;
	}
	/* Print regular output. */
	printf("%s\n", uri.url);
	printf("%s\n", uri.prot ? uri.prot : "");
	printf("%s\n", uri.host ? uri.host : "");
	printf("%s\n", uri.port ? uri.port : "");
	printf("%s\n", uri.path ? uri.path : "");
	printf("%s\n", uri.qstr ? uri.qstr : "");
	return 0;
}
