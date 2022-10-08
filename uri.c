#include <stdio.h>
#include <string.h>

#include "util.h"
#include "str.h"
#include "sb.h"
#include "uri.h"

enum _uri_2url
_uri_2url(Uri *uri, char *buf)
{
	int     len;		/* Lenght of normalized URL */

	len  = 0;
	buf[0] = '\0';

	if (uri->prot != NULL) {
		len += strlen(uri->prot) + 3;
		if (len > URI_MAX) return _URI_2URL_TOO_LONG;
		strcat(buf, uri->prot);
		strcat(buf, "://");
	}
	if (uri->host != NULL) {
		len += strlen(uri->host);
		if (len > URI_MAX) return _URI_2URL_TOO_LONG;
		strcat(buf, uri->host);
	}
	if (uri->port != NULL) {
		len += strlen(uri->port) + 1;
		if (len > URI_MAX) return _URI_2URL_TOO_LONG;
		strcat(buf, ":");
		strcat(buf, uri->port);
	}
	if (uri->path != NULL) {
		len += strlen(uri->path);
		if (len > URI_MAX) return _URI_2URL_TOO_LONG;
		strcat(buf, uri->path);
	}
	if (uri->qstr != NULL) {
		len += strlen(uri->qstr) + 1;
		if (len > URI_MAX) return _URI_2URL_TOO_LONG;
		strcat(buf, "?");
		strcat(buf, uri->qstr);
	}
	return _URI_2URL_OK;
}

enum uri_create
uri_create(Uri *uri, char *prot, char *host, char *port,
	   char *path, char *qstr)
{
	/* Manipulate _sb with SB. */
	Sb sb;

	memset(uri, 0, sizeof(Uri));

	sb_init(&sb, uri->_sb, URI_BSIZ);

	if (prot != NULL && (uri->prot = sb_add(&sb, prot)) == NULL)
		return URI_CREATE_PROT;

	if (host != NULL && (uri->host = sb_add(&sb, host)) == NULL)
		return URI_CREATE_HOST;

	if (port != NULL && (uri->port = sb_add(&sb, port)) == NULL)
		return URI_CREATE_PORT;

	if (path != NULL && (uri->path = sb_add(&sb, path)) == NULL)
		return URI_CREATE_PATH;

	if (qstr != NULL && (uri->qstr = sb_add(&sb, qstr)) == NULL)
		return URI_CREATE_QSTR;

	switch (_uri_2url(uri, uri->url)) {
	case _URI_2URL_OK: break;
	case _URI_2URL_TOO_LONG: return URI_CREATE_2URL;
	}

	return URI_CREATE_OK;
}

enum uri_parse
uri_parse(Uri *uri, char *src)
{
	/* BUF will contain content of SRC but with spaces replaced to
	 * %20.  BP is just to make manipulation of BUF easier and
	 * FOUND is used while searching in BUF.  SB is used to
	 * manipulate _sb of URI. */
	char buf[URI_BSIZ], *bp, *found;
	Sb sb;

	bp = buf;

	/* Too long after all " " are replaced to "%20". */
	if (str_nrep(src, " ", "%20", bp, URI_BSIZ) != 0)
		return URI_PARSE_TOO_LONG;

	memset(uri, 0, sizeof(Uri));

	sb_init(&sb, uri->_sb, URI_BSIZ);

	/* TODO(irek): It looks like right now parsing can fail only
	 * if URI in BUF is too long.  There are more cases when we
	 * should return error.  Right now those unhandled cases will
	 * propagate to later code and probably trigger errors while
	 * establishing connection or sending message to server.*/

	/* Find query string.  It's easier to start searching for URI
	 * parts from end of the BP string. */
        if ((found = strstr(bp, "?")) != NULL) {
		if (strlen(found) > 1) {
			uri->qstr = sb_add(&sb, found+1);
		}
		*found = '\0';	/* Cut off qstr from BP string */
	}

	/* Find protocol. */
        if ((found = strstr(bp, "://")) == NULL) {
		/* Use default protocol */
		uri->prot = sb_addn(&sb, URI_PROT, strlen(URI_PROT));
	} else {
		uri->prot = sb_addn(&sb, bp, found - bp);
		bp = found + 3;
	}

	/* Find path.  With protocol ahd his :// out the way we can go
	 * back to searching backwards which is easier. */
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

	switch (_uri_2url(uri, uri->url)) {
	case _URI_2URL_OK: break;
	case _URI_2URL_TOO_LONG: return URI_PARSE_TOO_LONG;
	}

	return URI_PARSE_OK;
}
