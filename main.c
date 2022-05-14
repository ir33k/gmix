#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define IMPLEMENTATION
#include "util.h"
#include "url.h"
#include "gmif.h"
#undef IMPLEMENTATION

/* Buffer Size should be at least 1024+6 bytes so it can hold whole
 * response header (1029 B) and single URL (1024 B). */

#define BSIZ (1024+6)	/* Buffer size */

int
main(int argc, char **argv)
{
	int     err;		/* Error code */
	char    buf[BSIZ];	/* Buffer */
	char   *bp;		/* Buffer Pointer */
	Urlv    urlv;		/* URL View */
	SSL    *ssl;		/* TLS connection */

	if (argc < 2) {
		printf("Usage: %s url\n", argv[0]);
		exit(1);
	}

	bp = buf;

	if (urlv_parse(argv[1], &urlv) != 0)
		die("urlv_parse: Invalid url %s", argv[1]);

	gmif_init();

	urlv_sprint(bp, &urlv);
	strcat(bp, "\r\n");

	ssl = gmif_open(urlv.host, urlv.port, bp);

	while ((err = gmif_gets(ssl, bp, BSIZ)) > 0)
		printf("%s", bp);

	printf("err: %d\n", err);

	printf("FIRST TRY!!!\n");
	return 0;
}
