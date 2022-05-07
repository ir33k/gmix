#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

/* For creating socket connection. */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORT	"1965"		/* Default Gemini port */

void
die(char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "ERR: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[strlen(fmt)-1] == ':')
		fprintf(stderr, " %s", strerror(errno));

	fprintf(stderr, "\n");

	exit(1);
}

/* Open socket to Gemini capsule with given URL (without gemini://
 * protocol prefix). */
int				/* Return Scoket File Descriptor */
gmi_connect(char *url)
{
	int     sfd;		/* Socket file descriptor */
	int     err;		/* For error code */

	/* HINT hints getaddrinfo that given URL is for IP TCP socket
	 * connection.  RES points at list of getaddrinfo results and
	 * RP (Results Pointer) is used to iterate over them. */
	struct addrinfo hint, *res, *rp;

	memset(&hint, 0, sizeof(hint));	/* Init with 0 */

	/* Combination of AF_INET, SOCK_STREAM and ai_protocol (as 0,
	 * because structure is init with 0) makes it IP TCP socket.
	 * Read ip(7) and tcp(7) man pages for more information.
	 * Other possible values are listed in socket(2) man page. */

	hint.ai_family   = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	/* TODO(irek): For now default Gemini protocol is used but
	 * Gemini allows for protocol to be defined in URL so it can
	 * be different for each connection. */

	if ((err = getaddrinfo(url, PORT, &hint, &res)) != 0)
		die("getaddrinfo: %s", gai_strerror(err));

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		if ((sfd = socket(rp->ai_family,
				  rp->ai_socktype,
				  rp->ai_protocol)) == -1)
			continue; /* Failure */

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* Success */

		close(sfd);
	}
	freeaddrinfo(res);	/* No longer needed */

	if (rp == NULL)
		die("Can't connect to %s", url);

	return sfd;
}

int
main(void)
{
	int     sfd;		/* Socket File Descriptor */

	sfd = gmi_connect("gemini.circumlunar.space");

	printf("Socket File Desciptor: %d\n", sfd);

	return 0;
}
