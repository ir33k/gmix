#include <assert.h>
#include <string.h>

#include "test.h"
#include "str.h"

#define BSIZ	64

IT("strnrep")
{
	char	buf[BSIZ];

	OK(strnrep("str ing with spa ces", " ", "%20", buf, BSIZ) == 0);
	BUF_EQ(buf, "str%20ing%20with%20spa%20ces", BSIZ);
	OK(strnrep("str ing with spa ces", " ", "%20", buf, 10) != 0);
}

int
main(void)
{
	return TEST();
}
