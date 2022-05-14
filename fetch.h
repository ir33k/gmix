/* Fetch Gemini server response like a file content */

/*
 * For simplest example usage read ./gmif.c program source.
 */

#ifndef FETCH_H_
#define FETCH_H_

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
int fetch__connect_tcp(char *host, char *port, int *sfd);

/*
 * Initialize OpenSSL library.  Should be called only once before
 * doing anything else with FETCH library or SSL library.  If SSL
 * library was already initialized by something else, then calling
 * this function can be skipped.  Details on OpenSSL can be found in
 * man pages that are available in libssl-doc package (on Debian).
 */
void fetch_init(void);

/*
 * Open SSL connection to given HOST with PORT.  Make initial call to
 * server with URL string that has to end with \r\n characters.  There
 * is no validation or parsing of HOST, PORT and URL.  Values must be
 * correct in order to open connection.  On error return NULL,
 * otherwise return pointer to SSL connection.
 */
SSL *fetch_open(char *host, char *port, char *url);

/*
 * Get response from SSL connection into BUF buffer of given SIZ size.
 * Response string is null-terminated.  Function can be called in loop
 * until return value is <=0.  Returned value is essentially returned
 * value of SSL_read.  That means on success it's value >0 being size
 * of read bytes.  On error it's <=0 where 0 means error because end
 * of response and values <0 are actual error codes that could
 * indicate the read process could be repeated.  On error, for either
 * end of response or other error, fetch_end is called.
 */
int fetch_gets(SSL *ssl, char *buf, int siz);

/*
 * End SSL connection by shutting it down and freeing memory allocated
 * once fetch_open was successfully called.  Calling this function is
 * optional.  If you managed to get whole response with fetch_gets then
 * this function was called automatically at the end anyway.  Calling
 * this function by hand might be useful if you like to end SSL
 * connection in middle of operation for some reason.
 */
void fetch_end(SSL *ssl);

#endif	/* FETCH_H_ */
