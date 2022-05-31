#include <assert.h>
#include <string.h>

#include "util.h"
#include "uri.h"

/* TODO(irek): I'm adding more and more tests.  It might be useful to
 * create small test.h lib for tests. */

/*
 * Exit with 1 and print error with CTX message for context if given
 * BOOL is not 1.
 */
void test_assert(char *ctx, int bool);

/*
 * Compare S1 with S2.  Exit with 1 and print error if strings are
 * different.  Use CTX string for context in error message.
 */
void test_str(char *ctx, char *s1, char *s2);

/*
 * Parse INPUT string with uri_parse function and compare result Uri
 * struct with given URI parts.
 */
void test_uri_parse(char *input, char *url, char *prot, char *host,
		    char *port, char *path, char *qstr);

/*
 * Test uri.h lib.
 */
int
main(void)
{
	test_uri_parse(
		"domain/file/path.gmi?some query",
		"gemini://domain:1965/file/path.gmi?some%20query",
		"gemini", "domain", "1965", "/file/path.gmi",
		"some%20query");

	test_uri_parse(
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query",
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query",
		"gemini", "gempaper.strangled.net", "1965",
		"/path/to/file.gmi", "some%20query");

	test_uri_parse(
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi?",
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi",
		"gemini", "gempaper.strangled.net", "1965", "/path/to/file.gmi",
		NULL);

	test_uri_parse(
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi",
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi",
		"gemini", "gempaper.strangled.net", "1965", "/path/to/file.gmi",
		NULL);

	test_uri_parse(
		"gemini://gempaper.strangled.net:1965/path/to/file",
		"gemini://gempaper.strangled.net:1965/path/to/file",
		"gemini", "gempaper.strangled.net", "1965", "/path/to/file",
		NULL);

	test_uri_parse(
		"gemini://gempaper.strangled.net:1965/",
		"gemini://gempaper.strangled.net:1965/",
		"gemini", "gempaper.strangled.net", "1965", "/",
		NULL);

	test_uri_parse(
		"gemini://gempaper.strangled.net:1965",
		"gemini://gempaper.strangled.net:1965/",
		"gemini", "gempaper.strangled.net", "1965", "/",
		NULL);

	test_uri_parse(
		"gemini://gempaper.strangled.net",
		"gemini://gempaper.strangled.net:1965/",
		"gemini", "gempaper.strangled.net", "1965", "/",
		NULL);

	test_uri_parse(
		"gempaper.strangled.net",
		"gemini://gempaper.strangled.net:1965/",
		"gemini", "gempaper.strangled.net", "1965", "/",
		NULL);

	test_uri_parse(
		"gempaper.strangled.net:1965/path/to/file.gmi?some query",
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query",
		"gemini", "gempaper.strangled.net", "1965", "/path/to/file.gmi",
		"some%20query");

	test_uri_parse(
		"gempaper.strangled.net/path/to/file.gmi?some%20query",
		"gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query",
		"gemini", "gempaper.strangled.net", "1965", "/path/to/file.gmi",
		"some%20query");

	test_uri_parse(
		"gempaper.strangled.net/path/",
		"gemini://gempaper.strangled.net:1965/path/",
		"gemini", "gempaper.strangled.net", "1965", "/path/",
		NULL);

	test_uri_parse(
		"gempaper.strangled.net/path",
		"gemini://gempaper.strangled.net:1965/path",
		"gemini", "gempaper.strangled.net", "1965", "/path",
		NULL);

	test_uri_parse(
		"gempaper.strangled.net/",
		"gemini://gempaper.strangled.net:1965/",
		"gemini", "gempaper.strangled.net", "1965", "/",
		NULL);

	test_uri_parse(
		"gempaper.strangled.net/?some%20query",
		"gemini://gempaper.strangled.net:1965/?some%20query",
		"gemini", "gempaper.strangled.net", "1965", "/",
		"some%20query");

	test_uri_parse(
		"gempaper.strangled.net?some%20query",
		"gemini://gempaper.strangled.net:1965/?some%20query",
		"gemini", "gempaper.strangled.net", "1965", "/",
		"some%20query");

	test_uri_parse(
		"gempaper.strangled.net:1965/?some%20query",
		"gemini://gempaper.strangled.net:1965/?some%20query",
		"gemini", "gempaper.strangled.net", "1965", "/",
		"some%20query");

	test_uri_parse(
		"gempaper.strangled.net:1965?some%20query",
		"gemini://gempaper.strangled.net:1965/?some%20query",
		"gemini", "gempaper.strangled.net", "1965", "/",
		"some%20query");

	test_uri_parse(
		"/",
		"gemini://:1965/",
		"gemini", NULL, "1965", "/",
		NULL);

	test_uri_parse(
		"/path/to/file.gmi",
		"gemini://:1965/path/to/file.gmi",
		"gemini", NULL, "1965", "/path/to/file.gmi",
		NULL);

	test_uri_parse(
		"/path/to/file",
		"gemini://:1965/path/to/file",
		"gemini", NULL, "1965", "/path/to/file",
		NULL);

	test_uri_parse(
		"/path/to/file.gmi?some%20query",
		"gemini://:1965/path/to/file.gmi?some%20query",
		"gemini", NULL, "1965", "/path/to/file.gmi",
		"some%20query");

	test_uri_parse(
		"/?some%20query",
		"gemini://:1965/?some%20query",
		"gemini", NULL, "1965", "/",
		"some%20query");

	test_uri_parse(
		"?some%20query",
		"gemini://:1965/?some%20query",
		"gemini", NULL, "1965", "/",
		"some%20query");

	return 0;
}

void
test_assert(char *ctx, int bool)
{
	if (bool != 1)
		die("Assert of '%s' failed", ctx);
}

void
test_str(char *ctx, char *s1, char *s2)
{
	if ((s1 == NULL && s2 != NULL) ||
	    (s1 != NULL && s2 == NULL) ||
	    (s1 != NULL && s2 != NULL && strcmp(s1, s2) != 0)) {
		die("For '%s' expected strings to be equal:\n"
		    "\t%s\n"
		    "\t%s", ctx, s1, s2);
	}
}

void
test_uri_parse(char *input, char *url, char *prot, char *host,
	       char *port, char *path, char *qstr)
{
	Uri	uri;

	test_assert(input, uri_parse(&uri, input) == 0);
	test_str(input, uri.url, url);
	test_str(input, uri.prot, prot);
	test_str(input, uri.host, host);
	test_str(input, uri.port, port);
	test_str(input, uri.path, path);
	test_str(input, uri.qstr, qstr);
}
