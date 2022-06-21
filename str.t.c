#include "walter.h"
#include "str.h"

#define BSIZ	64

TEST("strnrep")
{
	char	buf[BSIZ];

	OK(strnrep("str ing with spa ces", " ", "%20", buf, BSIZ) == 0);
	BUF_EQ(buf, "str%20ing%20with%20spa%20ces", BSIZ);

	OK(strnrep("beg AAA middle aaa AAA end", "AAA", "TEST", buf, BSIZ) == 0);
	BUF_EQ(buf, "beg TEST middle aaa TEST end", BSIZ);

	OK(strnrep("beg AAA middle aaa AAA end", "BBB", "TEST", buf, BSIZ) == 0);
	BUF_EQ(buf, "beg AAA middle aaa AAA end", BSIZ);

	OK(strnrep("str ing with spa ces", " ", "%20", buf, 10) != 0);
}
