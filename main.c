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
#include <openssl/err.h>

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

void
ssl_die(SSL_CTX *ctx)
{
	fprintf(stderr, "ERR: ssl: ");
	ERR_print_errors_fp(stderr);
	SSL_CTX_free(ctx);
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

int
main(void)
{
	int       sfd;		/* Socket File Descriptor */
	int       err;
	char      buf[BUFSIZ];
	char     *bp;
	SSL_CTX  *ctx;
	SSL      *ssl;

	char *url = "gemini.circumlunar.space";

	bp = buf;

	/* Hardcode default Gemini port for now */
	if ((err = connect_tcp(url, "1965", &sfd)) != 0)
		die("connect_tcp(%s:%s): %s", url, "1965",
		    err == 1 ? "Can't connect" : gai_strerror(err));

	/* On Debian libssl-doc package provides OpenSSL man pages. */
	/* Init OpenSSL */
	SSL_load_error_strings();
	SSL_library_init();

	/* SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE); */

	if ((ctx = SSL_CTX_new(TLS_client_method())) == NULL)
		ssl_die(ctx);

	if ((ssl = SSL_new(ctx)) == NULL)
		ssl_die(ctx);

	SSL_set_fd(ssl, sfd);

	if (SSL_connect(ssl) == -1)
		ssl_die(ctx);

	sprintf(buf, "gemini://%s/\r\n", url);
	if ((SSL_write(ssl, bp, sizeof(buf))) == 0)
		ssl_die(ctx);

	if ((SSL_read(ssl, bp, 1024)) == 0)
		ssl_die(ctx);

	printf("buf 1: %s\n", buf);

	if ((SSL_read(ssl, bp, 256)) == 0)
		ssl_die(ctx);

	printf("buf 2: %s\n", buf);

	if ((SSL_read(ssl, bp, 256)) == 0)
		ssl_die(ctx);

	printf("buf 3: %s\n", buf);

	SSL_CTX_free(ctx);

	printf("FIRST TRY!!!\n");
	return 0;
}
