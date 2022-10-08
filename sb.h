/* Strings Buffer - multiple \0 separated strings in single buffer */

/*
 * Can be used in situation when you have single buffer and you would
 * like to store multiple strings inside one after the other separated
 * by null character.
 *
 *	000000000000000000000000    // Empty buffer of given MAX size
 *	ssss0ssssss0sssss0000000    // Buffer with 3 strings
 *	^    ^      ^               // Returned pointers by sb_add
 */

#ifndef SB_H_
#define SB_H_

#include <string.h>

typedef struct {
	size_t   _max;		/* Maximum buffer size */
	char	*_beg;		/* Buffer beggining position*/
	char	*_end;		/* Current end position */
} Sb;

/*
 * Initialize SB Strings Buffer with BUF Buffer pointer of MAX size.
 */
void sb_init(Sb *sb, char *buf, size_t max);

/*
 * Get size of currently occupied space in SB buffer including end
 * NULL characters of all stored strings.
 */
size_t sb_siz(Sb *sb);

/*
 * Add new SRC string of size SIZ to end of SB Strings Buffer.  Return
 * pointer to added string or NULL on error which is lack of space in
 * SB buffer.
 */
char *sb_addn(Sb *sb, char *src, size_t siz);

/*
 * Same as sb_addn but SRC (null-terminated) string length is used.
 * Add new SRC string to end of SB Strings Buffer.  Return pointer to
 * added string or NULL on error which is lack of space in SB buffer.
 */
char *sb_add(Sb *sb, char *src);

/*
 * Remove all strings from SB buffer.  Old strings aren't actually
 * being removed but they will be overwritten once sb_add is used.
 */
void sb_clear(Sb *sb);

#endif	/* SB_H_ */
