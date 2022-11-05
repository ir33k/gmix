/* Parse, create and validate Gemini URI. */
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
#ifndef GMIU_H
#define GMIU_H
#include <stdio.h>
#include <string.h>

#define GMIU_MAX    1024          /* Gemini URL max length */
#define GMIU_BSIZ   (GMIU_MAX+1)  /* Buffer size that can hold URL */
#define GMIU_PROT   "gemini"      /* Gemini protocol without :// */
#define GMIU_PORT   "1965"        /* Default Gemini port */
#define GMIU_PATH   "/"           /* Gemini default root path */

/*
 * Strings Buffer - multiple \0 separated strings in single buffer.
 * Used to create internal _sb buffer in struct gmiu_uri.
 * 
 * Can be used in situation when you have single buffer and you would
 * like to store multiple strings inside one after the other separated
 * by null character.
 *
 *	000000000000000000000000    // Empty buffer of given MAX size
 *	ssss0ssssss0sssss0000000    // Buffer with 3 strings
 *	^    ^      ^               // Returned pointers by sb_add
 */
struct gmiu__sb {
	size_t   _max;		/* Maximum buffer size */
	char	*_beg;		/* Buffer beggining position*/
	char	*_end;		/* Current end position */
};
/* Structure fields are consider read only.  Use gmiu_parse to create
 * URI from URL like string.  Use gmiu_create when you want to set URI
 * parts directly.  Both functions will set all fields. */
struct gmiu_uri {
	char	url[GMIU_BSIZ]; /* Normalized URL */
	char	_sb[GMIU_BSIZ]; /* Private strings buffer */
	/*
	 * So called parts or components of URI.  All are just
	 * pointers to strings located in internal _sb buffer.
	 */
	char	*prot;          /* Protocol without "://" */
	char	*host;          /* AKA domain */
	char	*port;          /* Without prefix ":" */
	char	*path;          /* Often empty as root path "/" */
	char	*qstr;          /* Query string without "?" prefix */
};
enum gmiu__2url {
	GMIU__2URL_OK = 0,
	GMIU__2URL_TOO_LONG     /* URL longer than GMIU_MAX */
};
enum gmiu_create {
	GMIU_CREATE_OK = 0,
	GMIU_CREATE_PROT,       /* Failed on "prot" URI part */
	GMIU_CREATE_HOST,       /* Failed on "host" URI part */
	GMIU_CREATE_PORT,       /* Failed on "port" URI part */
	GMIU_CREATE_PATH,       /* Failed on "path" URI part */
	GMIU_CREATE_QSTR,       /* Failed on "qstr" URI part */
	GMIU_CREATE_2URL        /* Failed on "url"  URI part */
};
enum gmiu_parse {
	GMIU_PARSE_OK = 0,
	GMIU_PARSE_TOO_LONG	/* URL longer than GMIU_MAX */
};

/* Initialize SB Strings Buffer with BUF Buffer pointer of MAX
 * size. */
void gmiu__sb_init(struct gmiu__sb *sb, char *buf, size_t max);

/* Get size of currently occupied space in SB buffer including end
 * NULL characters of all stored strings. */
size_t gmiu__sb_siz(struct gmiu__sb *sb);

/* Add new SRC string of size SIZ to end of SB Strings Buffer.  Return
 * pointer to added string or NULL on error which is lack of space in
 * SB buffer. */
char *gmiu__sb_addn(struct gmiu__sb *sb, char *src, size_t siz);

/* Same as sb_addn but SRC (null-terminated) string length is used.
 * Add new SRC string to end of SB Strings Buffer.  Return pointer to
 * added string or NULL on error when lack of space in SB buffer. */
char *gmiu__sb_add(struct gmiu__sb *sb, char *src);

/* Remove all strings from SB buffer.  Old strings aren't actually
 * being removed but they will be overwritten once sb_add is used. */
void gmiu__sb_clear(struct gmiu__sb *sb);

/* In SRC string replace all occurrences of FROM string with TO string
 * and save result in DEST string of MAX size.  All strings are
 * null-terminated.  Return 0 on success, return <0 on error. */
int gmiu__str_nrep(char *src, char *from, char *to, char *dest, size_t max);

/* Take URI parts and print from them normalized URL in BUF string.
 * It's expected that BUF has size GMIU_MAX+1 or bigger. */
enum gmiu__2url gmiu__2url(struct gmiu_uri *uri, char *buf);

/* Create URI from given parts.  All parts are optional and could be
 * NULL.  Default values wihh NOT be used.  URI will be created exact
 * passed values so keep in mind to not pass "://" in PROT, ":" in
 * PORT and "?" in QSTR. */
enum gmiu_create gmiu_create(struct gmiu_uri *uri, char *prot, char *host,
			     char *port, char *path, char *qstr);

/* Parse SRC string to URI. */
enum gmiu_parse gmiu_parse(struct gmiu_uri *uri, char *src);

#endif	/* GMIU_H */
#ifdef GMIU_IMPLEMENTATION

void
gmiu__sb_init(struct gmiu__sb *sb, char *buf, size_t max)
{
	sb->_max = max;
	sb->_beg = buf;
	sb->_end = buf;
}

size_t
gmiu__sb_siz(struct gmiu__sb *sb)
{
	return sb->_end - sb->_beg;
}

char *
gmiu__sb_addn(struct gmiu__sb *sb, char *src, size_t siz)
{
	char   *res;		/* Point at added SRC string */

	if (sb->_max < (gmiu__sb_siz(sb) + siz + 1)) {
		return NULL;
	}
	res = sb->_end;
	strncpy(sb->_end, src, siz);
	sb->_end += siz;
	*sb->_end = 0;
	sb->_end += 1;
	return res;
}

char *
gmiu__sb_add(struct gmiu__sb *sb, char *src)
{
	return gmiu__sb_addn(sb, src, strlen(src));
}

void
gmiu__sb_clear(struct gmiu__sb *sb)
{
	sb->_end = sb->_beg;
}

int
gmiu__str_nrep(char *src, char *from, char *to, char *dest, size_t max)
{
	size_t i = 0;               /* Index for SRC string */
	size_t j = 0;               /* Index for DEST string */
	size_t flen = strlen(from);
	size_t tlen = strlen(to);

	while (src[i] != 0 && j < max) {
		if (strncmp(&src[i], from, flen) != 0) {
			dest[j++] = src[i++];
			continue;
		}
		if (j + tlen > max) {
			return -1;
		}
		dest[j] = 0;
		strcat(dest, to);
		i += flen;
		j += tlen;
	}
	if (src[i] != 0) {
		return -1;
	}
	dest[j] = 0;
	return 0;
}

enum gmiu__2url
gmiu__2url(struct gmiu_uri *uri, char *buf)
{
	unsigned int len = 0;    /* Lenght of normalized URL */

	buf[0] = 0;
	if (uri->prot) {
		len += strlen(uri->prot) + 3;
		if (len > GMIU_MAX) return GMIU__2URL_TOO_LONG;
		strcat(buf, uri->prot);
		strcat(buf, "://");
	}
	if (uri->host) {
		len += strlen(uri->host);
		if (len > GMIU_MAX) return GMIU__2URL_TOO_LONG;
		strcat(buf, uri->host);
	}
	if (uri->port) {
		len += strlen(uri->port) + 1;
		if (len > GMIU_MAX) return GMIU__2URL_TOO_LONG;
		strcat(buf, ":");
		strcat(buf, uri->port);
	}
	if (uri->path) {
		len += strlen(uri->path);
		if (len > GMIU_MAX) return GMIU__2URL_TOO_LONG;
		strcat(buf, uri->path);
	}
	if (uri->qstr) {
		len += strlen(uri->qstr) + 1;
		if (len > GMIU_MAX) return GMIU__2URL_TOO_LONG;
		strcat(buf, "?");
		strcat(buf, uri->qstr);
	}
	return GMIU__2URL_OK;
}

enum gmiu_create
gmiu_create(struct gmiu_uri *uri, char *prot, char *host, char *port,
	    char *path, char *qstr)
{
	/* Manipulate _sb with SB. */
	struct gmiu__sb sb;

	memset(uri, 0, sizeof(struct gmiu_uri));
	gmiu__sb_init(&sb, uri->_sb, GMIU_BSIZ);

	if (prot && !(uri->prot = gmiu__sb_add(&sb, prot))) {
		return GMIU_CREATE_PROT;
	}
	if (host && !(uri->host = gmiu__sb_add(&sb, host))) {
		return GMIU_CREATE_HOST;
	}
	if (port && !(uri->port = gmiu__sb_add(&sb, port))) {
		return GMIU_CREATE_PORT;
	}
	if (path && !(uri->path = gmiu__sb_add(&sb, path))) {
		return GMIU_CREATE_PATH;
	}
	if (qstr && !(uri->qstr = gmiu__sb_add(&sb, qstr))) {
		return GMIU_CREATE_QSTR;
	}
	switch (gmiu__2url(uri, uri->url)) {
	case GMIU__2URL_OK: break;
	case GMIU__2URL_TOO_LONG: return GMIU_CREATE_2URL;
	}
	return GMIU_CREATE_OK;
}

enum gmiu_parse
gmiu_parse(struct gmiu_uri *uri, char *src)
{
	/* BUF will contain content of SRC but with spaces replaced to
	 * %20.  BP is just to make manipulation of BUF easier and
	 * FOUND is used while searching in BUF.  SB is used to
	 * manipulate _sb of URI. */
	char buf[GMIU_BSIZ], *bp = buf, *found;
	struct gmiu__sb sb;

	/* Too long after all " " are replaced to "%20". */
	if (gmiu__str_nrep(src, " ", "%20", bp, GMIU_BSIZ) != 0) {
		return GMIU_PARSE_TOO_LONG;
	}
	memset(uri, 0, sizeof(struct gmiu_uri));
	gmiu__sb_init(&sb, uri->_sb, GMIU_BSIZ);
	/* Find query string.  It's easier to start searching for URI
	 * parts from end of the BP string. */
	if ((found = strstr(bp, "?")) != NULL) {
		if (strlen(found) > 1) {
			uri->qstr = gmiu__sb_add(&sb, found+1);
		}
		*found = 0;	/* Cut off qstr from BP string */
	}
	/* Find protocol. */
	if ((found = strstr(bp, "://"))) {
		uri->prot = gmiu__sb_addn(&sb, bp, found - bp);
		bp = found + 3;
	} else {
		/* Use default protocol */
		uri->prot = gmiu__sb_addn(&sb, GMIU_PROT, strlen(GMIU_PROT));
	}
	/* Find path.  With protocol ahd his :// out the way we can go
	 * back to searching backwards which is easier. */
	if ((found = strstr(bp, "/"))) {
		uri->path = gmiu__sb_add(&sb, found);
		*found = 0;	/* Cut off path from BP string */
	} else {
		/* Use default path */
		uri->path = gmiu__sb_addn(&sb, GMIU_PATH, strlen(GMIU_PATH));
	}
	/* Find port. */
	if ((found = strstr(bp, ":"))) {
		uri->port = gmiu__sb_add(&sb, found+1);
		*found = 0;	/* Cut off port from BP string */
	} else {
		/* Use default port */
		uri->port = gmiu__sb_addn(&sb, GMIU_PORT, strlen(GMIU_PORT));
	}
	/* What remainds is host. */
	if (strlen(bp) > 0) {
		uri->host = gmiu__sb_add(&sb, bp);
	}
	switch (gmiu__2url(uri, uri->url)) {
	case GMIU__2URL_OK: break;
	case GMIU__2URL_TOO_LONG: return GMIU_PARSE_TOO_LONG;
	}
	return GMIU_PARSE_OK;
}

#endif /* GMIU_IMPLEMENTATION */
