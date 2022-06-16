#include "walter.h"
#include "uri.h"

TEST("Should parse valid uri")
{
	Uri	uri;

	OK(uri_parse(&uri, "domain/file/path.gmi?some query") == 0);
	STR_EQ(uri.url, "gemini://domain:1965/file/path.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "domain");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/file/path.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file.gmi") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net:1965/") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net:1965") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gemini://gempaper.strangled.net") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gempaper.strangled.net") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gempaper.strangled.net:1965/path/to/file.gmi?some query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gempaper.strangled.net/path/to/file.gmi?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gempaper.strangled.net/path/") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gempaper.strangled.net/path") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gempaper.strangled.net/") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "gempaper.strangled.net/?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gempaper.strangled.net?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gempaper.strangled.net:1965/?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "gempaper.strangled.net:1965?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "/") == 0);
	STR_EQ(uri.url, "gemini://:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "/path/to/file.gmi") == 0);
	STR_EQ(uri.url, "gemini://:1965/path/to/file.gmi");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "/path/to/file") == 0);
	STR_EQ(uri.url, "gemini://:1965/path/to/file");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file");
	STR_EQ(uri.qstr, NULL);

	OK(uri_parse(&uri, "/path/to/file.gmi?some%20query") == 0);
	STR_EQ(uri.url, "gemini://:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "/?some%20query") == 0);
	STR_EQ(uri.url, "gemini://:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(uri_parse(&uri, "?some%20query") == 0);
	STR_EQ(uri.url, "gemini://:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");
}
