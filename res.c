#include <stdlib.h>

#include "res.h"

enum res
res__code(char *buf)
{
	enum res res;		/* Response Status Code as enum */

	res = atoi(buf);

	switch (res) {
	case RES_NUL:		/* Just for sake of completion */
	case RES_IN:
	case RES_PASS:
	case RES_OK:
	case RES_300:
	case RES_301:
	case RES_TMP:
	case RES_OUT:
	case RES_CGI:
	case RES_PROX:
	case RES_TRAFIC:
	case RES_ERR:
	case RES_404:
	case RES_GONE:
	case RES_NOPROX:
	case RES_BAD:
	case RES_CERT:
	case RES_UNAUTH:
	case RES_UNVAL:
		return res;
	}
	return RES_NUL;
}
