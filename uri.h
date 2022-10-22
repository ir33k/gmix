/* Parse, create and validate Gemini URI */

/*
 * URI stands fro "Uniform Resource Identifier".
 * URL stands for "Uniform Resource Locator".
 *
 * In order to stay close to Gemini protocol specification this lib is
 * called URI.  When "uri" keyword is used then it refers to main lib
 * Uri structure which defines all URI parts including normalized URL.
 * When "url" keyword is used it means normalized URI as string.
 */

/*
 * From Gemini protocol specification
 * https://gemini.circumlunar.space/docs/specification.gmi
 *
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

#define URI_MAX     1024	/* Gemini URL max length */
#define URI_BSIZ   (URI_MAX+1)	/* Buffer size that can hold URL */
#define URI_PROT   "gemini"	/* Gemini protocol without :// */
#define URI_PORT   "1965"	/* Default Gemini port */
#define URI_PATH   "/"		/* Gemini default root path */

/*
 * Structure fields are consider read only.  Use uri_parse to create
 * URI from URL like string.  Use uri_create when you want to set URI
 * parts directly.  Both functions will set all fields.
 */
typedef struct {
	char	url[URI_BSIZ];	/* Normalized URL */
	char	_sb[URI_BSIZ];	/* Private strings buffer */
	/*
	 * So called parts or components of URI.  All are just
	 * pointers to strings located in internal _sb buffer.
	 */
	char	*prot;		/* Protocol without "://" */
	char	*host;		/* AKA domain */
	char	*port;		/* Without prefix ":" */
	char	*path;		/* Often empty as root path "/" */
	char	*qstr;		/* Query string without "?" prefix */
} Uri;

enum _uri_2url {
	_URI_2URL_OK = 0,
	_URI_2URL_TOO_LONG	/* URL longer than URI_MAX */
};

enum uri_create {
	URI_CREATE_OK = 0,
	URI_CREATE_PROT,	/* Failed on "prot" URI part */
	URI_CREATE_HOST,	/* Failed on "host" URI part */
	URI_CREATE_PORT,	/* Failed on "port" URI part */
	URI_CREATE_PATH,	/* Failed on "path" URI part */
	URI_CREATE_QSTR,	/* Failed on "qstr" URI part */
	URI_CREATE_2URL		/* Failed on "url"  URI part */
};

enum uri_parse {
	URI_PARSE_OK = 0,
	URI_PARSE_TOO_LONG	/* URL longer than URI_MAX */
};

/*
 * Take URI parts and print from them normalized URL in BUF string.
 * It's expected that BUF has size URI_MAX+1 or bigger.
 */
enum _uri_2url _uri_2url(Uri *uri, char *buf);

/*
 * Create URI from given parts.  All parts are optional and could be
 * NULL.  Default values wihh NOT be used.  URI will be created exact
 * passed values so keep in mind to not pass "://" in PROT, ":" in
 * PORT and "?" in QSTR.
 */
enum uri_create uri_create(Uri *uri, char *prot, char *host,
			   char *port, char *path, char *qstr);

/*
 * Parse SRC string to URI.
 */
enum uri_parse uri_parse(Uri *uri, char *src);

#endif	/* URI_H_ */
