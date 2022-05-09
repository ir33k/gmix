/* For Gemini URL stuff. */

#ifndef URL_H_
#define URL_H_

#include <string.h>
#include <strings.h>

#define URL_MAX    1024         /* Gemini URL max length */
#define URL_BSIZ   1025         /* Buffer size that can hold URL */
#define URL_PROT   "gemini"     /* Gemini protocol without :// */
#define URL_PORT   "1965"       /* Default Gemini port */

/* Gemini URL has max lenght of 1024 bytes.  The idea is to have URL
 * in single buffer and set of pointers for each url part.  Parts will
 * be divided by \0 char.  Consider example where pointers are numbers
 * and \0 char is marked with ^.
 * 
 * 	gemini://domain:1234/some/path?query
 * 	1     ^  2     ^3   ^4        ^5    ^
 * 	
 * Ofc original URL string buffer will be destroyed by this process
 * but URL will still be easy to print as a whole if neede and it's
 * easy to look at each part.  Missing default values can be added at
 * the end of buffer.
 * 
 * 	domain/some/path?query gemini 1234
 * 	2     ^4        ^5    ^1     ^3   ^
 *
 * This is useful only for reading (viewing) URL.  Structure can't be
 * used to create new URL from parts.  That's why structure is called
 * Urlv, V for View.  Also structure don't have it's own buffer so it
 * is important to not modify buffer holding URL while using Urlv.
 * And it's assumed that buffer can hold URL_BSIZ bytes. */
typedef struct {
	char   *prot;		/* Protocol (without ://)*/
	char   *host;		/* AKA domain */
	char   *port;		/* Often not defined */
	char   *path;		/* Often empty as root path "/" */
	char   *query;		/* Without leading "?" */
} Urlv;

int urlv_parse(char *buf, Urlv *urlv);
void urlv_sprint(char *buf, Urlv *urlv);

#endif	/* URL_H_ */

#ifdef IMPLEMENTATION

/* Parse BUF buffer that holds URL to URLV URL view structure.  BUF
 * will be modified during parsing and pointers inside URLV struct
 * relies on this BUF to not be modified later.  Size in BUF will not
 * change tho but it's expected that it is at least URL_BSIZ big.
 * Return 0 on success, return -1 on failure. */
int
urlv_parse(char *buf, Urlv *urlv)
{
	size_t  len;		/* For lengths */
	char   *beg;		/* Beggining of searched string */
	char   *end;		/* Ending of BUF string */

	/* Clear structure. */
	memset(urlv, 0, sizeof(*urlv));

	/* Get and check length. */
	if ((len = strlen(buf)) > URL_MAX)
		return -1;

	/* TODO(irek): Some characters like spaces should be replaced
	 * according to Gemini URL specification.  It's better to do
	 * that before parsing to avoid changing chars position.*/

	/* TODO(irek): It looks like right now parsing can fail only
	 * if URL in BUF is too long.  There are more cases when we
	 * should return error.  Right now those unhandled cases will
	 * propagate to later code and probably trigger errors while
	 * establishing connection or sending message to server.*/

	/* Used later to add default values for protocol and port that
	 * might be not present in url because are not required but we
	 * want to have defaults for ease of use.  +1 because we need
	 * to keep null terminator. */
	end = buf + len + 1;

	/* Search for protocol and beggining of host. */
        if ((beg = strstr(buf, "://")) == NULL) {
		urlv->host = buf;
		/* Add default protocol. */
		sprintf(end, "%s", URL_PROT);
		urlv->prot = end;
		end += strlen(urlv->prot) + 1;
	} else {
		urlv->prot = buf;
		buf = beg+3;
		urlv->host = buf;
		*beg = '\0';
	}

	/* Search for port. */
        if ((beg = index(buf, ':')) == NULL) {
		/* Add default port. */
		sprintf(end, "%s", URL_PORT);
		urlv->port = end;
		end += strlen(urlv->port) + 1;
	} else {
		*beg = '\0';
		buf = beg+1;
		urlv->port = buf;
	}

	/* Search for path */
        if ((beg = index(buf, '/')) == NULL) {
		urlv->path = NULL;
	} else {
		*beg = '\0';
		buf = beg+1;
		urlv->path = buf;
	}

	/* Search for query */
        if ((beg = index(buf, '?')) == NULL) {
		urlv->query = NULL;
	} else {
		*beg = '\0';
		buf = beg+1;
		urlv->query = buf;
	}

	return 0;
}

/* Print given URL in for of URLV to BUF. */
void
urlv_sprint(char *buf, Urlv *urlv)
{
	sprintf(buf, "%s://%s:%s/",
		urlv->prot,
		urlv->host,
		urlv->port);

	if (urlv->path) {
		strcat(buf, urlv->path);
	}

	if (urlv->query) {
		strcat(buf, "?");
		strcat(buf, urlv->query);
	}
}

#endif	/* IMPLEMENTATION */
