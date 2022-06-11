#include "test.h"
#include "sb.h"

#define BSIZ    16

IT("Initialize properly")
{
	char     buf[BSIZ];
	Sb	 sb;		/* Strings Buffer data structure */

	sb_init(&sb, buf, BSIZ);

	NUM_EQ(sb._beg, buf);
	NUM_EQ(sb._end, buf);
	NUM_EQ(sb._max, BSIZ);
	NUM_EQ(sb_siz(&sb), 0);
}

IT("Adds strings to SB as long as possible")
{
	char     buf[BSIZ];
	Sb	 sb;
	char	*str[4];	/* Pointers for strings in BUF */

	sb_init(&sb, buf, BSIZ);

	NUM_EQ(str[0] = sb_add(&sb, "AAAA"), buf +  0);
	NUM_EQ(str[1] = sb_add(&sb, "BBBB"), buf +  5);
	NUM_EQ(str[2] = sb_add(&sb, "CCCC"), buf + 10);
	NUM_EQ(str[3] = sb_add(&sb, "D"), NULL);
	NUM_EQ(sb_siz(&sb), 15);
	STR_EQ(str[0], "AAAA");
	STR_EQ(str[1], "BBBB");
	STR_EQ(str[2], "CCCC");
	STR_EQ(str[3], NULL);
}

IT("Adds strings to SB after clear")
{
	char     buf[BSIZ];
	Sb	 sb;
	char	*str;		/* Pointers for string in BUF */

	sb_init(&sb, buf, BSIZ);
	sb_add(&sb, "AAAA");
	sb_add(&sb, "BBBB");
	sb_add(&sb, "CCCC");
	sb_clear(&sb);

	NUM_EQ(sb_siz(&sb), 0);
	NUM_EQ(sb._beg, sb._end);

	NUM_EQ(str = sb_add(&sb, "AAAA"), buf);
	STR_EQ(str, "AAAA");
	NUM_EQ(sb_siz(&sb), 5);
}

/* TODO(irek): Test sb_addn function. */
IT("Adds strings of given length") {}
