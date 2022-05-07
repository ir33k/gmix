#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

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

int
gmi_connect(char *url)
{
	int     sfd;		/* Socket file descriptor */
	int     err;		/* For error codes */

	struct addrinfo  hint;	/* To hint connection information */
	struct addrinfo *res;   /* Result of getaddrinfo */
	struct addrinfo *rp;	/* Result pointer */

	memset(&hint, 0, sizeof(hint)); /* Init with 0 */

	hint.ai_family   = AF_INET;	/* Internet Address Family */
	hint.ai_socktype = SOCK_STREAM; /* Two-way stream connection */

	/* Combination of AF_INET and SOCK_STREAM and with ai_protocol
	 * value set to 0 (default because whole structure is init
	 * with 0) it makes it IP TCP socket.  For more informations
	 * read ip(7) and tcp(7) man pages. */

	/* 1965 is hardcoded default gemini port */
	if ((err = getaddrinfo(url, "1965", &hint, &res)) != 0)
		die("getaddrinfo: %s", gai_strerror(err));

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family,
			     rp->ai_socktype,
			     rp->ai_protocol);

		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* Success */

		close(sfd);
	}

	freeaddrinfo(res);	/* No longer needed */

	if (rp == NULL)
		die("Could not connect");

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
