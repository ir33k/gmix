#include <stdio.h>
#include <string.h>

#include "util.h"
#include "sb.h"
#include "uri.h"

int
uri__2url(Uri *uri, char *buf)
{
	int     len;		/* Lenght of normalized URL */

	len  = 0;
	buf[0] = '\0';

	if (uri->prot != NULL) {
		len += strlen(uri->prot) + 3;
		if (len > URI_MAX) return -1;
		strcat(buf, uri->prot);
		strcat(buf, "://");
	}
	if (uri->host != NULL) {
		len += strlen(uri->host);
		if (len > URI_MAX) return -1;
		strcat(buf, uri->host);
	}
	if (uri->port != NULL) {
		len += strlen(uri->port) + 1;
		if (len > URI_MAX) return -1;
		strcat(buf, ":");
		strcat(buf, uri->port);
	}
	if (uri->path != NULL) {
		len += strlen(uri->path);
		if (len > URI_MAX) return -1;
		strcat(buf, uri->path);
	}
	if (uri->qstr != NULL) {
		len += strlen(uri->qstr) + 1;
		if (len > URI_MAX) return -1;
		strcat(buf, "?");
		strcat(buf, uri->qstr);
	}
	return 0;
}

int
uri_create(Uri *uri, char *prot, char *host, char *port,
	   char *path, char *qstr)
{
	Sb      sb;		/* Manipulate _buf with SB */

	memset(uri, 0, sizeof(Uri));

	sb_init(&sb, uri->_buf, URI_BSIZ);

	if (uri->prot != NULL)
		if ((uri->prot = sb_add(&sb, prot)) == NULL)
			return -1;

	if (uri->host != NULL)
		if ((uri->host = sb_add(&sb, host)) == NULL)
			return -1;

	if (uri->port != NULL)
		if ((uri->port = sb_add(&sb, port)) == NULL)
			return -1;

	if (uri->path != NULL)
		if ((uri->path = sb_add(&sb, path)) == NULL)
			return -1;

	if (uri->qstr != NULL)
		if ((uri->qstr = sb_add(&sb, qstr)) == NULL)
			return -1;

	if (uri__2url(uri, uri->url) != 0)
		return -1;

	return 0;
}

int
uri_parse(Uri *uri, char *src)
{
	Sb      sb;		/* Manipulate _buf with SB */
	char    buf[URI_BSIZ];	/* TMP buffer to avoid modifing SRC */
	char   *bp;
	char   *found;		/* Beggining of searched string */

	bp = buf;

	if (strnrep(src, " ", "%20", bp, URI_BSIZ) != 0)
		return -1;	/* Too long after ' ' replacement */

	memset(uri, 0, sizeof(Uri));

	sb_init(&sb, uri->_buf, URI_BSIZ);

	/* TODO(irek): It looks like right now parsing can fail only
	 * if URI in BUF is too long.  There are more cases when we
	 * should return error.  Right now those unhandled cases will
	 * propagate to later code and probably trigger errors while
	 * establishing connection or sending message to server.*/

	/* Find query string.  It's easier to do it backwards. */
        if ((found = strstr(bp, "?")) != NULL) {
		if (strlen(found) > 1) {
			uri->qstr = sb_add(&sb, found+1);
		}
		*found = '\0';	/* Cut off qstr from BP string */
	}

	/* Search for protocol and beggining of host. */
        if ((found = strstr(bp, "://")) == NULL) {
		/* Use default protocol */
		uri->prot = sb_addn(&sb, URI_PROT, strlen(URI_PROT));
	} else {
		uri->prot = sb_addn(&sb, bp, found - bp);
		bp = found + 3;
	}

	/* With protocol with his :// out the way we can go back to
	 * searching backwards which is easier. */
        if ((found = strstr(bp, "/")) == NULL) {
		/* Use default path */
		uri->path = sb_addn(&sb, URI_PATH, strlen(URI_PATH));
	} else {
		uri->path = sb_add(&sb, found);
		*found = '\0';	/* Cut off path from BP string */
	}

	/* Find port. */
        if ((found = strstr(bp, ":")) == NULL) {
		/* Use default port */
		uri->port = sb_addn(&sb, URI_PORT, strlen(URI_PORT));
	} else {
		uri->port = sb_add(&sb, found+1);
		*found = '\0';	/* Cut off port from BP string */
	}

	/* What remainds is host. */
	if (strlen(bp) > 0) {
		uri->host = sb_add(&sb, bp);
	}

	if (uri__2url(uri, uri->url) != 0)
		return -1;

	return 0;
}
