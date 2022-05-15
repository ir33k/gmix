/* Strings Buffer - multiple \0 separated strings in single buffer */

#ifndef SB_H_
#define SB_H_

#include <string.h>

typedef struct {
	int	 max;		/* Maximum buffer size */
	char	*beg;		/* Buffer beggining position*/
	char	*end;		/* Current end position */
} Sb;

void	sb_set(Sb *, char *, size_t);
int	sb_siz(Sb *);
int	sb_add(Sb *, char *);
void	sb_del(Sb *);

#endif	/* SB_H_ */

#ifdef IMPLEMENTATION

/* Setup Strings Buffer that SB Strings Buffer Pointer points to with
 * given BUF buffer of given MAX size. */
void
sb_set(Sb *sb, char *buf, size_t max)
{
	sb->max = max;
	sb->beg = buf;
	sb->end = buf;
}

/* Get size of currently occupied space in SB buffer. */
int
sb_siz(Sb *sb)
{
	return sb->end - sb->beg;
}

/* Add new SRC string to end of SB Strings Buffer.  Return 0 on
 * success.  Otherwise return error code. */
int
sb_add(Sb *sb, char *src)
{
	size_t	len;		/* SRC string length */
	size_t  siz;		/* SB size */

	len = strlen(src);
	siz = sb_siz(sb);

	if (siz + len + 1 > sb->max)
		return -1;

	strcpy(sb->end, src);
	sb->end += len + 1;

	return 0;
}

/* Remove all strings from SB buffer.  Old strings aren't actually
 * being removed but they will be overwritten once sb_add is used. */
void
sb_del(Sb *sb)
{
	sb->end = sb->beg;
}

#endif	/* IMPLEMENTATION */
