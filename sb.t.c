#include <stdio.h>
#include <assert.h>

#include "sb.h"

void test(char *buf, size_t bsiz);

int
main(void)
{
	char	 buf1[16];	/* Buffer for SB */
	char	 buf2[16];	/* Buffer for SB */

	test(buf1, 16);

	/* Test how SB buffer behaves if given buffer has some old
	 * values.  It should act like it was empty all along. */
	sprintf(buf2, "xxxxxxxxxxxxxxx");
	test(buf2, 16);

	return 0;
}

void
test(char *buf, size_t bsiz)
{
	Sb	 sb;		/* Strings Buffer data structure */
	char	*str[4];	/* Pointers for strings in BUF */

	sb_init(&sb, buf, bsiz);

	assert(sb._beg == buf);
	assert(sb._end == buf);
	assert(sb._max == bsiz);
	assert(sb_siz(&sb) == 0);

	assert((str[0] = sb_add(&sb, "AAAA")) == buf +  0);
	assert((str[1] = sb_add(&sb, "BBBB")) == buf +  5);
	assert((str[2] = sb_add(&sb, "CCCC")) == buf + 10);
	assert((str[3] = sb_add(&sb, "D"))    == NULL);
	assert(sb_siz(&sb) == 15);

	sb_clear(&sb);
	assert(sb_siz(&sb) == 0);
	assert(sb._beg == sb._end);

	assert((str[0] = sb_addn(&sb, "AAAAAA", 4)) == buf + 0);
	assert(sb_siz(&sb) == 5);
}
