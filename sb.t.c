#include "walter.h"
#include "sb.h"

#define BSIZ    16

TEST("sb_init")
{
	char     buf[BSIZ];
	Sb	 sb;		/* Strings Buffer data structure */

	sb_init(&sb, buf, BSIZ);

	OK(sb._beg == buf);
	OK(sb._end == buf);
	OK(sb._max == BSIZ);
	OK(sb_siz(&sb) == 0);
}

TEST("sb_add")
{
	char     buf[BSIZ];
	Sb	 sb;
	char	*str[4];	/* Pointers for strings in BUF */

	sb_init(&sb, buf, BSIZ);

	OK((str[0] = sb_add(&sb, "AAAA")) == buf +  0);
	OK((str[1] = sb_add(&sb, "BBBB")) == buf +  5);
	OK((str[2] = sb_add(&sb, "CCCC")) == buf + 10);
	OK((str[3] = sb_add(&sb, "D")) == NULL);
	OK(sb_siz(&sb) == 15);
	STR_EQ(str[0], "AAAA");
	STR_EQ(str[1], "BBBB");
	STR_EQ(str[2], "CCCC");
	STR_EQ(str[3], NULL);
}

TEST("sb_clear")
{
	char     buf[BSIZ];
	Sb	 sb;
	char	*str;		/* Pointers for string in BUF */

	sb_init(&sb, buf, BSIZ);
	sb_add(&sb, "AAAA");
	sb_add(&sb, "BBBB");
	sb_add(&sb, "CCCC");
	sb_clear(&sb);

	OK(sb_siz(&sb) == 0);
	OK(sb._beg == sb._end);

	OK((str = sb_add(&sb, "AAAA")) == buf);
	STR_EQ(str, "AAAA");
	OK(sb_siz(&sb) == 5);
}

TEST("sb_addn")
{
	char     buf[BSIZ];
	Sb	 sb;
	char	*str[3];	/* Pointers for string in BUF */

	sb_init(&sb, buf, BSIZ);

	OK((str[0] = sb_addn(&sb, "ABCDEFG", 3)) != NULL);
	OK((str[1] = sb_addn(&sb, "abcdefg", 4)) != NULL);
	OK((str[2] = sb_addn(&sb, "0123456", 5)) != NULL);

	STR_EQ(str[0], "ABC");
	STR_EQ(str[1], "abcd");
	STR_EQ(str[2], "01234");

	OK(sb_siz(&sb) == 15);
}
