/* Parse Gemini response header. */

#ifndef GMIR_H
#define GMIR_H

/* Header Response Status Codes.  Some of them uses HTTP status codes
 * as a name because those are very common and easy to understand. */
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

/* Return non 0 value if STR string is response header. */
int gmir_valid(char *str);

/* Return enum representation of CODE, default to GMIR_NUL. */
enum gmir_code gmir_get(int code);

#endif	/* GMIR_H */
#ifdef GMIR_IMPLEMENTATION

int
gmir_valid(char *str)
{
	/* TODO(irek): Implement. */
	return 0;
}

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

#endif /* GMIR_IMPLEMENTATION */
