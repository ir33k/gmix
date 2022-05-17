#include <stdio.h>

#include "util.h"
#include "uri.h"

/* TODO(irek): Define flag "--fetch" or/and "-f" that makes program
 * output URL in format used by gmif program so I can run:
 * 
 * 	$ gmif `gmiu some-url`
 */

/*
 * Parse given URL and print all URI parts.
 */
int
main(int argc, char **argv)
{
	Uri     uri;

	if (argc < 2)
		die("usage: %s uri", argv[0]);

	if (uri_parse(&uri, argv[1]) != 0)
		die("Parse failed");

	printf("%s\n", uri.url  == NULL ? "" : uri.url);
	printf("%s\n", uri.prot == NULL ? "" : uri.prot);
	printf("%s\n", uri.host == NULL ? "" : uri.host);
	printf("%s\n", uri.path == NULL ? "" : uri.port);
	printf("%s\n", uri.path == NULL ? "" : uri.path);
	printf("%s\n", uri.qstr == NULL ? "" : uri.qstr);

	return 0;
}
