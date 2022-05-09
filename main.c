#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>

#define IMPLEMENTATION
#include "url.h"
#undef IMPLEMENTATION

/* Buffer Size should be at least 1024+6 bytes so it can hold whole
 * response header (1029 B) and single URL (1024 B). */

#define BSIZ	1024+6		/* Buffer size */

/* Print message with given FMT format like in printf and exit program
 * with error code 1.  If FMT string ends with ':' then after message
 * print strerror of current errno code. */
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

/* Open TCP IP socket connection to HOST with PORT.  On succes return
 * 0 and set SFD Socket File Descriptor.  If returned value is 1 then
 * connection can't be established.  Any other value is getaddrinfo
 * error code.
 *
 * Actually, according to man page, getaddrinfo returns any value
 * different that 0 as error.  So it could by 1 in theory.  But after
 * checking source code I found that all errors have negative values.
 * I need to distinguish between getaddrinfo erros and connection
 * error in single int value so I can print proper error message
 * outside this function.  I dediced that value of 1 is connection
 * error and all other are getaddrinfo errors. */
int
connect_tcp(char *host, char *port, int *sfd)
{
	int err;
	struct addrinfo hint, *res, *rp;

	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	if ((err = getaddrinfo(host, port, &hint, &res)) != 0)
		return err;

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		if ((*sfd = socket(rp->ai_family,
				   rp->ai_socktype,
				   rp->ai_protocol)) == -1)
			continue;

		if (connect(*sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* Success */

		close(*sfd);
	}
	freeaddrinfo(res);

	return rp == NULL ? 1 : 0;
}

/* Initialize OpenSSL.  Call this function before doing anything with
 * SSL library.  Man pages are in libssl-doc (Debian). */
void
ssl_init()
{
	SSL_load_error_strings();
	SSL_library_init();
}

/* Create TLS connection for SFC Socket File Descriptor connection.
 * Return pointer to SSL on success, return NULL on failuere. */
SSL *
ssl_new(int sfd)
{
	SSL_CTX  *ctx;
	SSL      *ssl;

	if ((ctx = SSL_CTX_new(TLS_client_method())) == NULL)
		return NULL;

	if ((ssl = SSL_new(ctx)) == NULL) {
		SSL_CTX_free(ctx);
		return NULL;
	}

	SSL_CTX_free(ctx);
	SSL_set_fd(ssl, sfd);

	if (SSL_connect(ssl) == -1) {
		SSL_free(ssl);
		return NULL;
	}

	return ssl;
}

/* Shutdown given SSL connection and clear SSL memory. */
void
ssl_end(SSL *ssl)
{
	if (ssl != NULL) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
}

int
main(int argc, char **argv)
{
	int     sfd;		/* Socket File Descriptor */
	int     err;		/* For error codes */
	char    buf[BSIZ];	/* Buffer */
	char   *bp;		/* Buffer Pointer */
	Urlv    urlv;		/* URL View */
	SSL    *ssl;		/* TLS connection */

	if (argc < 2) {
		printf("Usage: $ gmic url\n");
		exit(1);
	}

	bp = buf;

	if (urlv_parse(argv[1], &urlv) != 0)
		die("urlv_parse: Invalid url %s", argv[1]);

	ssl_init();

	if ((err = connect_tcp(urlv.host, urlv.port, &sfd)) != 0)
		die("connect_tcp (%s:%s): %s", urlv.host, urlv.port,
		    err == 1 ? "Can't connect" : gai_strerror(err));

	urlv_sprint(bp, &urlv);

	if ((ssl = ssl_new(sfd)) == NULL)
		die("ssl_new: Can't connect to %s", bp);

	strcat(bp, "\r\n");
	printf(">>> %s\n", bp);

	if ((SSL_write(ssl, bp, strlen(bp))) == 0) {
		ssl_end(ssl);
		die("SSL_write");
	}
	
	if ((SSL_read(ssl, bp, BSIZ)) == 0) {
		ssl_end(ssl);
		die("SSL_read");
	}

	printf("buf 1: %s\n", bp);

	if ((SSL_read(ssl, bp, BSIZ)) == 0) {
		ssl_end(ssl);
		die("SSL_read");
	}

	printf("buf 2: %s\n", bp);

	ssl_end(ssl);

	printf("FIRST TRY!!!\n");
	return 0;
}
