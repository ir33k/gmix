#include "walter.h"
#define GMIU_IMPLEMENTATION
#include "gmiu.h"

TEST("uri__sb_init")
{
	char buf[16];
	struct gmiu__sb sb;	/* Strings Buffer data structure */

	gmiu__sb_init(&sb, buf, 16);

	OK(sb._beg == buf);
	OK(sb._end == buf);
	OK(sb._max == 16);
	OK(gmiu__sb_siz(&sb) == 0);
}

TEST("gmiu__sb_add")
{
	char buf[16], *bp[4];
	struct gmiu__sb sb;

	gmiu__sb_init(&sb, buf, 16);

	OK((bp[0] = gmiu__sb_add(&sb, "AAAA")) == buf +  0);
	OK((bp[1] = gmiu__sb_add(&sb, "BBBB")) == buf +  5);
	OK((bp[2] = gmiu__sb_add(&sb, "CCCC")) == buf + 10);
	OK((bp[3] = gmiu__sb_add(&sb, "D")) == NULL);
	OK(gmiu__sb_siz(&sb) == 15);
	STR_EQ(bp[0], "AAAA");
	STR_EQ(bp[1], "BBBB");
	STR_EQ(bp[2], "CCCC");
	STR_EQ(bp[3], NULL);
}

TEST("gmiu__sb_clear")
{
	char buf[16], *bp;
	struct gmiu__sb sb;

	gmiu__sb_init(&sb, buf, 16);
	gmiu__sb_add(&sb, "AAAA");
	gmiu__sb_add(&sb, "BBBB");
	gmiu__sb_add(&sb, "CCCC");
	gmiu__sb_clear(&sb);

	OK(gmiu__sb_siz(&sb) == 0);
	OK(sb._beg == sb._end);

	OK((bp = gmiu__sb_add(&sb, "AAAA")) == buf);
	STR_EQ(bp, "AAAA");
	OK(gmiu__sb_siz(&sb) == 5);
}

TEST("gmiu__sb_addn")
{
	char buf[16], *bp[3];
	struct gmiu__sb sb;

	gmiu__sb_init(&sb, buf, 16);

	OK((bp[0] = gmiu__sb_addn(&sb, "ABCDEFG", 3)) != NULL);
	OK((bp[1] = gmiu__sb_addn(&sb, "abcdefg", 4)) != NULL);
	OK((bp[2] = gmiu__sb_addn(&sb, "0123456", 5)) != NULL);

	STR_EQ(bp[0], "ABC");
	STR_EQ(bp[1], "abcd");
	STR_EQ(bp[2], "01234");

	OK(gmiu__sb_siz(&sb) == 15);
}

TEST("gmiu__str_nrep")
{
	char buf[64];

	OK(gmiu__str_nrep("str ing with spa ces", " ", "%20", buf, 64) == 0);
	BUF_EQ(buf, "str%20ing%20with%20spa%20ces", 64);

	OK(gmiu__str_nrep("beg AAA middle aaa AAA end", "AAA", "TEST", buf, 64) == 0);
	BUF_EQ(buf, "beg TEST middle aaa TEST end", 64);

	OK(gmiu__str_nrep("beg AAA middle aaa AAA end", "BBB", "TEST", buf, 64) == 0);
	BUF_EQ(buf, "beg AAA middle aaa AAA end", 64);

	OK(gmiu__str_nrep("str ing with spa ces", " ", "%20", buf, 10) != 0);
}

TEST("gmiu_parse")
{
	struct gmiu_uri uri;

	OK(gmiu_parse(&uri, "domain/file/path.gmi?some query") == 0);
	STR_EQ(uri.url, "gemini://domain:1965/file/path.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "domain");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/file/path.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file.gmi") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net:1965/path/to/file") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net:1965/") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net:1965") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gemini://gempaper.strangled.net") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gempaper.strangled.net") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gempaper.strangled.net:1965/path/to/file.gmi?some query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gempaper.strangled.net/path/to/file.gmi?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gempaper.strangled.net/path/") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gempaper.strangled.net/path") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/path");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gempaper.strangled.net/") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "gempaper.strangled.net/?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gempaper.strangled.net?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gempaper.strangled.net:1965/?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "gempaper.strangled.net:1965?some%20query") == 0);
	STR_EQ(uri.url, "gemini://gempaper.strangled.net:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, "gempaper.strangled.net");
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "/") == 0);
	STR_EQ(uri.url, "gemini://:1965/");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "/path/to/file.gmi") == 0);
	STR_EQ(uri.url, "gemini://:1965/path/to/file.gmi");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "/path/to/file") == 0);
	STR_EQ(uri.url, "gemini://:1965/path/to/file");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file");
	STR_EQ(uri.qstr, NULL);

	OK(gmiu_parse(&uri, "/path/to/file.gmi?some%20query") == 0);
	STR_EQ(uri.url, "gemini://:1965/path/to/file.gmi?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/path/to/file.gmi");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "/?some%20query") == 0);
	STR_EQ(uri.url, "gemini://:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");

	OK(gmiu_parse(&uri, "?some%20query") == 0);
	STR_EQ(uri.url, "gemini://:1965/?some%20query");
	STR_EQ(uri.prot, "gemini");
	STR_EQ(uri.host, NULL);
	STR_EQ(uri.port, "1965");
	STR_EQ(uri.path, "/");
	STR_EQ(uri.qstr, "some%20query");
}
