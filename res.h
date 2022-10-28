/* Parse Gemini response header */

#ifndef RES_H_
#define RES_H_

/* Header Response Status Codes.  Some of them uses HTTP status codes
 * as a name because those are very common and easy to understand. */
enum res {
	RES_NUL    =  0,	/* Unknown status code */
				/* 1x INPUT */
	RES_IN     = 10,	/* Input, like search phrase */
	RES_PASS   = 11,	/* Sensitive input, like password */
	                        /* 2X SUCCESS */
	RES_OK     = 20,	/* All good */
	                        /* 3x Redirection */
	RES_300    = 30,	/* Temporary redirection */
	RES_301    = 31,	/* Permanent redirection */
	                        /* 4X TMP FAIL */
	RES_TMP    = 40,	/* Temporary failure */
	RES_OUT    = 41,	/* Server unavailable */
	RES_CGI    = 42,	/* CGI error */
	RES_PROX   = 43,	/* Proxy error */
	RES_TRAFIC = 44,	/* Rate limiting, you have to wait */
	                        /* 5x PERMANENT FAIL */
	RES_ERR    = 50,	/* Permanent failure */
	RES_404    = 51,	/* Not found */
	RES_GONE   = 52,	/* Resource no longer available */
	RES_NOPROX = 53,	/* Proxy request refused */
	RES_BAD    = 59,	/* Bad requaest */
	                        /* 6X CLIENT CERT */
	RES_CERT   = 60,	/* Client certificate required */
	RES_UNAUTH = 61,	/* Certificate not authorised */
	RES_UNVAL  = 62		/* Cerfiticate not valid */
} RES;

/*
 * Get header line status code.  Return RES_NUL if invalid.
 */
enum res res__code(char *buf);

#endif	/* RES_H_ */
