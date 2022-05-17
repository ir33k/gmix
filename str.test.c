#include <assert.h>
#include <string.h>

#include "str.h"

#define BSIZ	64

int
main(void)
{
	char	buf[BSIZ];

	assert(strnrep("str ing with spa ces", " ", "%20", buf, BSIZ) == 0);
	assert(strncmp(buf, "str%20ing%20with%20spa%20ces", BSIZ) == 0);

	return 0;
}
