/* Gemini Fetch - Get Gemini capsule (page) content. */
#include <stdio.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIF_IMPLEMENTATION
#include "gmif.h"

/* Buffer size should be at least 1024+3 bytes so it can hold whole
 * URL (1024 B) with end \r\n chars and null-terminated char.. */

#define BSIZ     2048		/* Buffer size */
#define PORT    "1965"		/* Default Gemini port */

int
main(int argc, char **argv)
{
	/* SIZ is size of data or error code.  BUF for URL and output.
	 * SSL for TLS connection. */
	int siz;
	char buf[BSIZ];
	SSL *ssl;

	if (getopt(argc, argv, "h") != -1 ||
	    argc < 3) {
		die("(GMI Fetch) Fetch Gemini server HOST:PORT with given URL.\n"
		    "Server response with response header is printed in stdout.\n\n"
		    "\tusage: %s url host [port]\n\n"
		    "PORT is optional, defaults to 1965.  There's no URL parsing\n"
		    "or validation.  Which is great for precise test of server\n"
		    "response.\n",
		    argv[0]);
	}
	/* Build resource URL with required suffix \r\n. */
	sprintf(buf, "%s\r\n", argv[1]);
	/* Init GMIF, open connection and get response data. */
	gmif_init();
	if ((ssl = gmif_open(argv[2], argc > 3 ? argv[3] : PORT, buf)) == NULL) {
		die("Can't open connection");
	}
	while ((siz = gmif_gets(ssl, buf, BSIZ)) > 0) {
		fwrite(buf, 1, siz, stdout);
	}
	/* End program with error code 1 in case of negative size
	 * which indicates error while reading response. */
	return siz < 0;
}
