/* Parse and validate Gemini URI (Uniform Resource Identifier) */

/*
 * 1.2 Gemini URI scheme
 * 
 * Resources hosted via Gemini are identified using URIs with the
 * scheme "gemini".  This scheme is syntactically compatible with the
 * generic URI syntax defined in RFC 3986, but does not support all
 * components of the generic syntax.  In particular, the authority
 * component is allowed and required, but its userinfo subcomponent is
 * NOT allowed.  The host subcomponent is required.  The port
 * subcomponent is optional, with a default value of 1965.  The path,
 * query and fragment components are allowed and have no special
 * meanings beyond those defined by the generic syntax.  An empty path
 * is equivalent to a path consisting only of "/".  Spaces in paths
 * should be encoded as %20, not as +.
 * 
 * Clients SHOULD normalise URIs (as per section 6.2.3 of RFC 3986)
 * before sending requests (see section 2) and servers SHOULD
 * normalise received URIs before processing a request.
 *
 * <URL> is a UTF-8 encoded absolute URL, including a scheme, of
 * maximum length 1024 bytes.  The request MUST NOT begin with a
 * U+FEFF byte order mark.
 */

#ifndef URI_H_
#define URI_H_

#define URI_MAX     1024	/* Gemini URI max length */
#define URI_BSIZ   (URI_MAX+1)	/* Buffer size that can hold URI */
#define URI_PROT   "gemini"	/* Gemini protocol without :// */
#define URI_PORT   "1965"	/* Default Gemini port */
#define URI_PATH   "/"		/* Gemini default root path */

typedef struct {
	char     url[URI_BSIZ];	/* Normalized URL */
	char    _buf[URI_BSIZ];	/* Private buffer for URI parts */
	/*
	 * So called parts or components of URI.  All are just
	 * pointers to strings located in internal _buf buffer.
	 */
	char    *prot;		/* Protocol without "://" */
	char    *host;		/* AKA domain */
	char    *port;		/* Without prefix ":" */
	char    *path;		/* Often empty as root path "/" */
	char    *qstr;		/* Query string without "?" prefix */
} Uri;

/*
 * Take URI parts and print from them normalized URL in BUF string.
 * It's expected that BUF has size URI_MAX+1 or bigger.  Return 0 on
 * success, return <0 on error.
 */
int uri__2url(Uri *uri, char *buf);

/*
 * Create URI from given parts.  All parts are optional and could be
 * NULL.  Default values wihh NOT be used.  URI will be created exact
 * passed values so keep in mind to not pass "://" in PROT, ":" in
 * PORT and "?" in QSTR.  Return 0 on success.
 */
int uri_create(Uri *uri, char *prot, char *host, char *port,
	       char *path, char *qstr);

/*
 * Parse SRC string to URI.  Return 0 on success.
 */
int uri_parse(Uri *uri, char *src);

#endif	/* URI_H_ */
