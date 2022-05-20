#include <string.h>

#include "sb.h"
#include "str.h"

void
sb_init(Sb *sb, char *buf, size_t max)
{
	sb->_max = max;
	sb->_beg = buf;
	sb->_end = buf;
}

size_t
sb_siz(Sb *sb)
{
	return sb->_end - sb->_beg;
}

char *
sb_addn(Sb *sb, char *src, size_t siz)
{
	char   *res;		/* Point at added SRC string */

	if (sb->_max < (sb_siz(sb) + siz + 1))
		return NULL;

	res = sb->_end;
	strncpy(sb->_end, src, siz);
	sb->_end += siz;
	*sb->_end = '\0';
	sb->_end += 1;

	return res;
}

char *
sb_add(Sb *sb, char *src)
{
	return sb_addn(sb, src, strlen(src));
}

void
sb_clear(Sb *sb)
{
	sb->_end = sb->_beg;
}
