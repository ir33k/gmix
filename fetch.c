/*
 * -D_POSIX_C_SOURCE=200112L flag in compiler or this define below is
 * necessary to programs that include netdb.h because of problem with
 * addrinfo struct of getaddrinfo.
 */
#define _POSIX_C_SOURCE 200112L

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/* TODO(irek): OpenSSL provides a lot of features that I'm not going
 * to use.  It's heavy and slow down compilation.  Also I found out
 * that it's not trivial to compile with OpenSSL on MacOS.  I would
 * like to replace it with something else.  I don't know if I'm
 * capable of writing my own code that deals with all of that what is
 * required for Gemini.  I might investigate this problem later. */
#include <openssl/ssl.h>

#include "fetch.h"

enum _fetch_tcp
_fetch_tcp(char *host, char *port, int *sfd)
{
	int err;
	struct addrinfo hint, *res, *rp;

	memset(&hint, 0, sizeof(hint));

	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	/* TOOD(irek): If not for potential error in getaddrinfo then
	 * I could just return int as SFD instead of passing pointer
	 * to it as third argument. */
	if ((err = getaddrinfo(host, port, &hint, &res)) != 0)
		return err;

	for (rp = res; rp; rp = rp->ai_next) {
		if ((*sfd = socket(rp->ai_family,
				   rp->ai_socktype,
				   rp->ai_protocol)) == -1)
			continue;

		if (connect(*sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* Success */

		close(*sfd);
	}
	freeaddrinfo(res);

	return rp == NULL ? _FETCH_TCP_ERR : _FETCH_TCP_OK;
}

void
fetch_init(void)
{
	/* TODO(irek): Use OPENSSL_init_ssl instead? */
	SSL_library_init();
}

SSL *
fetch_open(char *host, char *port, char *url)
{
	int        sfd;		/* Socket File Descriptor */
	SSL_CTX   *ctx;		/* SSL Context */
	SSL       *ssl;		/* SSL connection */

	switch (_fetch_tcp(host, port, &sfd)) {
	case _FETCH_TCP_OK: break;
	case _FETCH_TCP_ERR:
		return NULL;
	default:		/* getaddrinfo error */
		/* TODO(irek): I'm not handling getaddrinfo errors
		 * anyway.  Should I then simplify _fetch_tcp function
		 * ro should I handle all errors?  What to do... */
		return NULL;
	}

	if ((ctx = SSL_CTX_new(TLS_client_method())) == NULL)
		return NULL;

	if ((ssl = SSL_new(ctx)) == NULL)
		return NULL;

	/* TODO(irek): Ok so I just did this whole SSL stuff the
	 * simplest way that I could do with my lack of knowledge at
	 * that time.  I'm sure that it's not production ready
	 * implementation and that it does not follow specification or
	 * even common sense.  I'm not planning of changing that any
	 * time soon because my current goal is to make gmic program.
	 * So let's first make it work, then make it correct and at
	 * the end make it pretty. */

	SSL_CTX_free(ctx);
	SSL_set_fd(ssl, sfd);

	if (SSL_connect(ssl) == -1) {
		SSL_free(ssl);
		return NULL;
	}
	if (SSL_write(ssl, url, strlen(url)) == 0) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		return NULL;
	}
	return ssl;
}

int
fetch_gets(SSL *ssl, char *buf, int siz)
{
	int     res;		/* Buf size or err code */

	if ((res = SSL_read(ssl, buf, siz)) <= 0)
		fetch_end(ssl);

	return res;
}

void
fetch_end(SSL *ssl)
{
	if (ssl != NULL) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
	}
}
