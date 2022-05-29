#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "util.h"
#include "uri.h"

/*
 * Print program usage instruction using provided progrm NAME.
 */
void usage(char *name);

/*
 * Parse given URL and print all URI parts.
 */
int
main(int argc, char **argv)
{
	Uri	uri;
	char	opt;

	if (argc < 2) {
		usage(argv[0]);
		putchar('\n');
		die("Missing argument");
	}

	if (uri_parse(&uri, argv[argc-1]) != 0)
		die("Parse failed");

	/* Print output for gmif program if -f option is provided. */
	while ((opt = getopt(argc, argv, "f")) != -1) {
		if (opt == 'f') {
			printf("%s %s %s",
			       uri.url  == NULL ? "" : uri.url,
			       uri.host == NULL ? "" : uri.host,
			       uri.port == NULL ? "" : uri.port);
			return 0;	/* End here */
		}
	}

	/* Else print everything */
	printf("%s\n", uri.url  == NULL ? "" : uri.url);
	printf("%s\n", uri.prot == NULL ? "" : uri.prot);
	printf("%s\n", uri.host == NULL ? "" : uri.host);
	printf("%s\n", uri.port == NULL ? "" : uri.port);
	printf("%s\n", uri.path == NULL ? "" : uri.path);
	printf("%s\n", uri.qstr == NULL ? "" : uri.qstr);

	return 0;
}

void
usage(char *name)
{
	printf("(GMI URI) Parse Gemeni URL and print result in stdout.\n\n"
	       "\tusage: %s [-f] uri\n\n"
	       "Parsed URI is printed in normalized URL format, and with\n"
	       "all URI parts: protocol, hostname, port, path and query.\n"
	       "If -f option is provided then output is printed in format\n"
	       "prepared or gmif progrm.\n",
	       name);
}
