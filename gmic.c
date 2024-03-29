#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIU_IMPLEMENTATION
#include "gmiu.h"
#define GMIF_IMPLEMENTATION
#include "gmif.h"

/* Buffer Size should be at least 1024+6 bytes so it can hold whole
 * response header (1029 B) and single URL (1024 B). */

#define BSIZ    (1024+6)	/* Buffer size */

int
main(int argc, char **argv)
{
	int siz;		/* For size and error code */
	char buf[BSIZ];		/* Buffer */
	struct gmiu_uri uri;	/* URL View */
	SSL *ssl;		/* TLS connection */

	if (argc < 2) {
		die("usage: %s url", argv[0]);
	}
	switch (gmiu_parse(&uri, argv[1])) {
	case GMIU_PARSE_OK: break;
	case GMIU_PARSE_TOO_LONG: die("Parsed url is too long");
	}
	printf("uri.url     %s\n", uri.url);
	printf("uri._sb     ");
	fwrite(uri._sb, 1, strlen(uri.url), stdout);
	printf("\n");
	printf("uri.prot    %s\n", uri.prot);
	printf("uri.host    %s\n", uri.host);
	printf("uri.port    %s\n", uri.port);
	printf("uri.path    %s\n", uri.path);
	printf("uri.query   %s\n", uri.qstr);
	gmif_init();
	sprintf(buf, "%s\r\n", uri.url);

	if ((ssl = gmif_open(uri.host, uri.port, buf)) == NULL) {
		die("Can't open connection");
	}
	while ((siz = gmif_gets(ssl, buf, BSIZ)) > 0) {
		fwrite(buf, 1, siz, stdout);
	}
	return siz < 0;		/* Return 1 on error */
}
