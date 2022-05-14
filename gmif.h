/* Gemini Fetch - get Gemini server response like a file content */

/*
 * For simplest example usage read ./gmif.c program source.
 */

/*
 * It's a single header library so to include implementation part
 * along with declarations you have to define IMPLEMENTATION before
 * including library.  You can undefine that afterwards.  Include
 * implementation only in one of your program files.  In other files
 * include lib normally.  Example with including mulitple single
 * header libraries:
 *
 *	#define IMPLEMENTATION
 *	#include "lib0.h"
 *	#include "lib1.h"
 *	#include "lib2.h"
 *	#undef IMPLEMENTATION
 */

#ifndef GMIF_H_
#define GMIF_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>

/*
 * Open TCP IP socket connection to HOST with PORT.  On succes return
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
 * error and all other are getaddrinfo errors.
 */
int gmif__connect_tcp(char *host, char *port, int *sfd);

/*
 * Initialize OpenSSL library.  Should be called only once before
 * doing anything else with GMIF library or SSL library.  If SSL
 * library was already initialized by something else, then calling
 * this function can be skipped.  Details on OpenSSL can be found in
 * man pages that are available in libssl-doc package (on Debian).
 */
void gmif_init(void);

/*
 * Open SSL connection to given HOST with PORT.  Make initial call to
 * server with URL string that has to end with \r\n characters.  There
 * is no validation or parsing of HST, PORT and URL.  Values must be
 * correct in order to open connection.  On error return NULL,
 * otherwise return pointer to SSL connection.
 */
SSL *gmif_open(char *host, char *port, char *url);

/*
 * Get response from SSL connection into BUF buffer of given SIZ size.
 * Response string is null-terminated.  Function can be called in loop
 * until return value is <=0.  Returned value is essentially returned
 * value of SSL_read.  That means on success it's value >0 being size
 * of read bytes.  On error it's <=0 where 0 means error because end
 * of response and values <0 are actual error codes that could
 * indicate the read process could be repeated.  On error, for either
 * end of response or other error, gmif_end is called.
 */
int gmif_gets(SSL *ssl, char *buf, int siz);

/*
 * End SSL connection by shutting it down and freeing memory allocated
 * once gmif_open was successfully called.  Calling this function is
 * optional.  If you managed to get whole response with gmif_gets then
 * this function was called automatically at the end anyway.  Calling
 * this function by hand might be useful if you like to end SSL
 * connection in middle of operation for some reason.
 */
void gmif_end(SSL *ssl);

#endif	/* GMIF_H_ */

#ifdef IMPLEMENTATION

int
gmif__connect_tcp(char *host, char *port, int *sfd)
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

void
gmif_init(void)
{
	/* TODO(irek): Use OPENSSL_init_ssl instead? */
	SSL_library_init();
}

SSL *
gmif_open(char *host, char *port, char *url)
{
	int        sfd;		/* Socket File Descriptor */
	SSL_CTX   *ctx;		/* SSL Context */
	SSL       *ssl;		/* SSL connection */

	if (gmif__connect_tcp(host, port, &sfd) != 0)
		return NULL;

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
gmif_gets(SSL *ssl, char *buf, int siz)
{
	int     res;		/* Buf size or err code */

	if ((res = SSL_read(ssl, buf, siz)) <= 0)
		gmif_end(ssl);

	return res;
}

void
gmif_end(SSL *ssl)
{
	if (ssl != NULL) {
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
	}
}

#endif	/* IMPLEMENTATION */
