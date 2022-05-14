#include <stdio.h>
#include <string.h>
#include "util.h"
#include "uri.h"

int
uri__2url(Uri *uri, char *buf)
{
	int     len;		/* Lenght of normalized URL */

	len  = 0;
	len += strlen(uri->prot);
	len += 3;		/* For "://" */
	len += strlen(uri->host);
	len += 1;		/* For ":" */
	len += strlen(uri->port);
	len += strlen(uri->path);

	if (uri->query) {
		len += 1;	/* For "?" */
		len += strlen(uri->query);
	}

	if (len > URI_MAX)
		return -1;

	sprintf(buf, "%s://%s:%s%s",
		uri->prot,
		uri->host,
		uri->port,
		uri->path);

	if (uri->query) {
		strcat(buf, "?");
		strcat(buf, uri->query);
	}

	return 0;
}

int
uri_parse(char *str, Uri *uri)
{
	size_t  len;		/* To track length */
	char   *beg;		/* Beggining of searched string */
	char   *end;		/* Ending of uri->_buf */
	char   *bp;		/* Points at uri->_buf */

	memset(uri, 0, sizeof(Uri));

	bp = uri->_buf;		/* Just for ease of use */

	/*
	 * Basic idea of this function is to put original STR into
	 * internal uri->_buf buffer with potential spaces replaced
	 * with "%20" string.  Internal buffer if used to store
	 * strings for each URI component, and each one is separated
	 * from another with \0 sign.  Pointers of Uri structure
	 * points at each component in that internal buffer.  Then
	 * internal buffer is scanned looking for each URI component
	 * that was already present in original STR.  If component is
	 * not found but has default value then default value is added
	 * at the end of internal buffer.  At the end when all
	 * components are defined then final uri->url is build.
	 */

	if (strnrep(str, " ", "%20", bp, URI_BSIZ) != 0)
		return -1;	/* Too long after ' ' replacement */

	len = strlen(bp);

	/* TODO(irek): It looks like right now parsing can fail only
	 * if URI in BUF is too long.  There are more cases when we
	 * should return error.  Right now those unhandled cases will
	 * propagate to later code and probably trigger errors while
	 * establishing connection or sending message to server.*/

	/* Used later to add default values for protocol and port that
	 * might be not present in URI because are not required but we
	 * want to have defaults for ease of use.  +1 because we need
	 * to keep null terminator. */
	end = bp + len + 1;

	/* Search for protocol and beggining of host. */
        if ((beg = strstr(bp, "://")) == NULL) {
		uri->host = bp;
		/* Add default protocol. */
		sprintf(end, "%s", URI_PROT);
		uri->prot = end;
		end += strlen(uri->prot) + 1;
	} else {
		uri->prot = bp;
		bp = beg+3;
		uri->host = bp;
		*beg = '\0';
	}

	/* Search for port. */
        if ((beg = strchr(bp, ':')) == NULL) {
		/* Add default port. */
		sprintf(end, "%s", URI_PORT);
		uri->port = end;
		end += strlen(uri->port) + 1;
	} else {
		*beg = '\0';
		bp = beg+1;
		uri->port = bp;
	}

	/* Search for path */
        if ((beg = strchr(bp, '/')) == NULL) {
		/* Add default path. */
		sprintf(end, "%s", URI_PATH);
		uri->path = end;
		end += strlen(uri->path) + 1;
	} else {
		*beg = '\0';
		bp = beg+1;
		uri->path = bp;
	}

	/* Search for query */
        if ((beg = strchr(bp, '?')) == NULL) {
		uri->query = NULL;
	} else {
		*beg = '\0';
		bp = beg+1;
		uri->query = bp;
	}

	if (uri__2url(uri, uri->url) != 0)
		return -1;

	return 0;
}
