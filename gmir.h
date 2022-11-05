/* Parse Gemini response header. */
/*
 * Gemini response consist of a single CRLF-terminated header line,
 * optionally followed by a response body.  Response headers:
 * 
 *    - Is a two-digit numeric status code.
 *    - Is a single space character, i.e. the byte 0x20.
 *    - Is a UTF-8 encoded string of maximum length 1024 bytes,
 *      whose meaning is dependent.
 * 
 * The response header as a whole and as a sub-string both MUST NOT
 * begin with a U+FEFF byte order mark.
 * 
 * If does not belong to the "SUCCESS" range of codes, then the server
 * MUST close the connection after sending the header and MUST NOT
 * send a response body.
 * 
 * If a server sends a which is not a two-digit number or a which
 * exceeds 1024 bytes in length, the client SHOULD close the
 * connection and disregard the response header, informing the user
 * of an error.
 */
#ifndef GMIR_H
#define GMIR_H

#define GMIR_LEN     (2+1+1024) /* Max response header length */

/* Header Response Status Codes. */
enum gmir_code {
	GMIR_NUL          =  0, /* Unknown status code */
	                        /* 1X INPUT */
	GMIR_INPUT_TEXT   = 10, /* Regular input, search phrase */
	GMIR_INPUT_PASS   = 11, /* Sensitive input, password */
	                        /* 2X SUCCESS */
	GMIR_OK           = 20, /* All good */
	                        /* 3X Redirection */
	GMIR_REDIR_TEMP   = 30, /* Temporary redirection */
	GMIR_REDIR_PERM   = 31, /* Permanent redirection */
	                        /* 4X TMP FAIL */
	GMIR_WARN_TEMP    = 40, /* Temporary failure */
	GMIR_WARN_OUT     = 41, /* Server unavailable */
	GMIR_WARN_CGI     = 42, /* CGI error */
	GMIR_WARN_PROX    = 43, /* Proxy error */
	GMIR_WARN_LIMIT   = 44, /* Rate limiting, you have to wait */
	                        /* 5X PERMANENT FAIL */
	GMIR_ERR_PERM     = 50, /* Permanent failure */
	GMIR_ERR_404      = 51, /* Not found */
	GMIR_ERR_GONE     = 52, /* Resource no longer available */
	GMIR_ERR_NOPROX   = 53, /* Proxy request refused */
	GMIR_ERR_BAD      = 59, /* Bad requaest */
	                        /* 6X CLIENT CERT */
	GMIR_CERT_REQU    = 60, /* Client certificate required */
	GMIR_CERT_UNAUTH  = 61, /* Certificate not authorised */
	GMIR_CERT_INVALID = 62  /* Cerfiticate not valid */
};
enum gmir_valid {
	GMIR_VALID_OK = 0,	/* Response header is valid */
	GMIR_VALID_LEN,		/* Too long */
	GMIR_VALID_FEFF,        /* Forbidden first char U+FEFF */
	GMIR_VALID_CODE,	/* Invalid code */
	GMIR_VALID_CRLF,	/* Missing \r\n at the end */
	GMIR_VALID_WS		/* Missing space separator */
};

/* Return enum representation of CODE, default to GMIR_NUL. */
enum gmir_code gmir_get(int code);

/* Return 0 for STR that is valid response header. */
enum gmir_valid gmir_valid(char *str);

#endif	/* GMIR_H */
#ifdef GMIR_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>

enum gmir_code
gmir_get(int code)
{
	switch (code) {
	case GMIR_NUL:
	case GMIR_INPUT_TEXT:
	case GMIR_INPUT_PASS:
	case GMIR_OK:
	case GMIR_REDIR_TEMP:
	case GMIR_REDIR_PERM:
	case GMIR_WARN_TEMP:
	case GMIR_WARN_OUT:
	case GMIR_WARN_CGI:
	case GMIR_WARN_PROX:
	case GMIR_WARN_LIMIT:
	case GMIR_ERR_PERM:
	case GMIR_ERR_404:
	case GMIR_ERR_GONE:
	case GMIR_ERR_NOPROX:
	case GMIR_ERR_BAD:
	case GMIR_CERT_REQU:
	case GMIR_CERT_UNAUTH:
	case GMIR_CERT_INVALID:
		return code;
	}
	return GMIR_NUL;
}

enum gmir_valid
gmir_valid(char *str)
{
	size_t len = strlen(str);
	char code[3] = "00";

	if (len > GMIR_LEN) {
		return GMIR_VALID_LEN;
	}
	if (!strncmp(str, "\xFE\xFF", 2)) {
		return GMIR_VALID_FEFF;
	}
	if (str[2] != ' ') {
		return GMIR_VALID_WS;
	}
	if (str[len-1] != '\n') {
		return GMIR_VALID_CRLF;
	}
	if (str[len-2] != '\r') {
		return GMIR_VALID_CRLF;
	}
	strncpy(code, str, 2);
	if (!gmir_get(atoi(code))) {
		return GMIR_VALID_CODE;
	}
	return GMIR_VALID_OK;
}

#endif /* GMIR_IMPLEMENTATION */
