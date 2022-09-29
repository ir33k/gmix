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
	default:
		return NULL;
	}

	if ((ctx = SSL_CTX_new(TLS_client_method())) == NULL)
		return NULL;

	if ((ssl = SSL_new(ctx)) == NULL)
		return NULL;

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
