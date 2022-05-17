#include <stdio.h>

#include "util.h"
#include "fetch.h"

/* Buffer size should be at least 1024+3 bytes so it can hold whole
 * URL (1024 B) with end \r\n chars and null-terminated char.. */

#define BSIZ     2048		/* Buffer size */
#define PORT    "1965"		/* Default Gemini port */

char *help =
	"(GMI Fetch) Fetch Gemini server HOST:PORT with given URL.\n\n"
	"usage: %s url host [port]\n\n"
	"PORT is optional, defaults to 1965.  There's no URL parsing\n"
	"or validation.  What you define as URL will be sent to server,\n"
	"which is great for precise test of server response.  Output\n"
	"includes and starts with full response header.";

int
main(int argc, char **argv)
{
	int     siz;		/* Size of data or error code */
	char    buf[BSIZ];	/* Buffer for URL and output */
	SSL    *ssl;		/* TLS connection */

	if (argc < 3)
		die(help, argv[0]);

	fetch_init();

	/* Build resource URL with suffix \r\n. */
	sprintf(buf, "%s\r\n", argv[1]);

	if ((ssl = fetch_open(argv[2], argc > 3 ? argv[3] : PORT, buf)) == NULL)
		die("Can't open connection");

	while ((siz = fetch_gets(ssl, buf, BSIZ)) > 0)
		fwrite(buf, 1, siz, stdout);

	/* End program with error code 1 in case of negative size
	 * which indicates error while reading response. */
	return siz < 0;
}
