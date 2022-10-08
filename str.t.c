#include "walter.h"
#include "str.h"

#define BSIZ	64

TEST("str_nrep")
{
	char buf[BSIZ];

	OK(str_nrep("str ing with spa ces", " ", "%20", buf, BSIZ) == 0);
	BUF_EQ(buf, "str%20ing%20with%20spa%20ces", BSIZ);

	OK(str_nrep("beg AAA middle aaa AAA end", "AAA", "TEST", buf, BSIZ) == 0);
	BUF_EQ(buf, "beg TEST middle aaa TEST end", BSIZ);

	OK(str_nrep("beg AAA middle aaa AAA end", "BBB", "TEST", buf, BSIZ) == 0);
	BUF_EQ(buf, "beg AAA middle aaa AAA end", BSIZ);

	OK(str_nrep("str ing with spa ces", " ", "%20", buf, 10) != 0);
}

TEST("str_triml")
{
	char *str0 = "aaa";
	char *str1 = "    aaa			";
	char *str2 = "				aaa	";
	char *str3 = " 	 	aaa   ";
	char *str4 = "";

	STR_EQ(str_triml(str0), str0);	   /* Skip nothing */
	STR_EQ(str_triml(str1), str1 + 4); /* Skip 4 spaces */
	STR_EQ(str_triml(str2), str2 + 4); /* Skip 4 tabs */
	STR_EQ(str_triml(str3), str3 + 4); /* Skip 2 spaces and 2 tabs */
	STR_EQ(str_triml(str4), str4);	   /* Handle empty string */
}
