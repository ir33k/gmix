#include <stdlib.h>
#include <string.h>
#include "walter.h"
#define GMIR_IMPLEMENTATION
#include "gmir.h"

TEST("gmir_get")
{
	/* Valid. */
	OK(gmir_get(GMIR_NUL)          == GMIR_NUL);
	OK(gmir_get(GMIR_INPUT_TEXT)   == GMIR_INPUT_TEXT);
	OK(gmir_get(GMIR_INPUT_PASS)   == GMIR_INPUT_PASS);
	OK(gmir_get(GMIR_OK)           == GMIR_OK);
	OK(gmir_get(GMIR_REDIR_TEMP)   == GMIR_REDIR_TEMP);
	OK(gmir_get(GMIR_REDIR_PERM)   == GMIR_REDIR_PERM);
	OK(gmir_get(GMIR_WARN_TEMP)    == GMIR_WARN_TEMP);
	OK(gmir_get(GMIR_WARN_OUT)     == GMIR_WARN_OUT);
	OK(gmir_get(GMIR_WARN_CGI)     == GMIR_WARN_CGI);
	OK(gmir_get(GMIR_WARN_PROX)    == GMIR_WARN_PROX);
	OK(gmir_get(GMIR_WARN_LIMIT)   == GMIR_WARN_LIMIT);
	OK(gmir_get(GMIR_ERR_PERM)     == GMIR_ERR_PERM);
	OK(gmir_get(GMIR_ERR_404)      == GMIR_ERR_404);
	OK(gmir_get(GMIR_ERR_GONE)     == GMIR_ERR_GONE);
	OK(gmir_get(GMIR_ERR_NOPROX)   == GMIR_ERR_NOPROX);
	OK(gmir_get(GMIR_ERR_BAD)      == GMIR_ERR_BAD);
	OK(gmir_get(GMIR_CERT_REQU)    == GMIR_CERT_REQU);
	OK(gmir_get(GMIR_CERT_UNAUTH)  == GMIR_CERT_UNAUTH);
	OK(gmir_get(GMIR_CERT_INVALID) == GMIR_CERT_INVALID);
	/* Invalid. */
	OK(gmir_get(12)  == GMIR_NUL);
	OK(gmir_get(13)  == GMIR_NUL);
	OK(gmir_get(123) == GMIR_NUL);
}

TEST("gmir_valid")
{
	/* For header that is longer than it should be. */
	char *str = malloc(2048);

	/* Everything is valid about that STR response header
	 * including code itself and ending chracters except for that
	 * it's one character too long. */
	strcat(str, "20 ");
	strcat(str, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); /* 256 */
	strcat(str, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	strcat(str, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	strcat(str, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); /* 254 */
	/* Just one too much. */
	strcat(str, "b");
	strcat(str, "\r\n");

	/* Valid. */
	OK(gmir_valid("20 gemini://test\r\n") == GMIR_VALID_OK);
	/* Invalid. */
	OK(gmir_valid(str) == GMIR_VALID_LEN);
	OK(gmir_valid("12 gemini://te\r\n") == GMIR_VALID_CODE);
	OK(gmir_valid("20 gemini://test\n") == GMIR_VALID_CRLF);
	OK(gmir_valid("20 gemini://test\r") == GMIR_VALID_CRLF);
	OK(gmir_valid("20 gemini://te\n\r") == GMIR_VALID_CRLF);
	OK(gmir_valid("20\tgemini://t\n\r") == GMIR_VALID_WS);
	OK(gmir_valid("020 gemini://t\n\r") == GMIR_VALID_WS);
}
