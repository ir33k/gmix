#include <string.h>

#include "str.h"

int
strnrep(char *src, char *from, char *to, char *dest, size_t max)
{
	size_t  i;		/* Index for SRC string */
	size_t  j;		/* Index for DEST string */
	size_t  flen;		/* FROM string length */
	size_t  tlen;		/* TO string length */

	i = 0;
	j = 0; 
	flen = strlen(from);
	tlen = strlen(to);

	while (src[i] != '\0' && j < max) {
		if (strncmp(&src[i], from, flen) != 0) {
			dest[j] = src[i];
			j++;
			i++;
			continue;
		}
		if (j + tlen > max) {
			return -1;
		}
		dest[j] = '\0';
		strcat(dest, to);

		i += flen;
		j += tlen;
	}
	if (src[i] != '\0') {
		return -1;
	}
	dest[j] = '\0';

	return 0;
}
