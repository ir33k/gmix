/* Parser Gemini text/gemini format.  Allows to easily go through
 * stream filling given buffer peace by peace along with struct gmip
 * that will tell what kind of line is it and at which part of line
 * parsing we are.  For simplest example see gmip.c program. */

#ifndef GMIP_H
#define GMIP_H
#include <stdio.h>

#define GMIP__URL_MAX       1024+1 /* GMI URL max length */
#define GMIP__MIN_PREF      5      /* Min prefix size + null */
#define GMIP__2STDOUT_PAD   8      /* Padding for stdout prefixes */
#define GMIP__2MD_PAD       6      /* Padding for Markdown */
#define GMIP__2HTML_PAD     46     /* Padding for HTML tags */

/* Line types.  NUL should never be returned.  It's used internally by
 * gmip function.  ALL line types ends with '\n' except for URL and
 * PRE.  First can end with '\n' but can also end with ' ' or '\t'.
 * DSC is always returned as next type after URL.  DSC is optional so
 * if it's not provided STR (from gmip function) will be empty.  PRE
 * ends with "```" and it's the only line type that will include new
 * line characters and whitespaces in STR. */
enum gmip_lt {
	GMIP_NUL = 0,		/* Empty type */
	GMIP_H1,		/* Heading level 1 */
	GMIP_H2,		/* Heading level 2 */
	GMIP_H3,		/* Heading level 3 */
	GMIP_P,			/* Regular text, paragraph, default */
	GMIP_LI,		/* Unordered list item */
	GMIP_Q,			/* Quote */
	GMIP_URL,		/* URL link */
	GMIP_DSC,		/* URL description */
	GMIP_PRE		/* Preformatted text, no formatting */
};

/* Parse State.  All values are read only.  Initialize struct with 0.
 * URL is last parsed url.  This can be used as default DSC when DSC
 * is empty.  OLD and NEW are previous and current Line Type.  BEG is
 * non 0 when NEW line just started and EOL is last line end character
 * which is non 0 when end of line of current NEW type was reached. */
struct gmip {
	char url[GMIP__URL_MAX];
	enum gmip_lt old, new;
	int beg;
	char eol;
};

/* Prepend SRC to DSC of SIZ.  Return 0 when SRC wont fit. */
int gmip__prepend(char *dsc, size_t siz, char *src);

/* Append SRC to DSC of SIZ.  Return 0 when SRC wont fit. */
int gmip__append(char *dsc, size_t siz, char *src);

/* Forward position in FP stream until first character not mentioned
 * in SKIP string is reached.  Return that character, might be EOF. */
int gmip__fskip(FILE *fp, char *skip);

/* Get current line type from FP stream.  Function expects that
 * current stream position is at the beginning of file, end of line or
 * beginning of new line.  STR string has to be at least 5 bytes long
 * and will contain loaded characters, that is leading line prefix or
 * first few characters of paragraph.  DSC is never returned because
 * this line type is forced in gmip function always after URL. */
enum gmip_lt gmip__lt(char *str, FILE *fp);

/* Main parsing function.  With 0 initialized PS parser state put as
 * much bytes into STR string of SIZ size from FP stream.  Each time
 * function is run values in PS are updated to reflect state of
 * content in STR.  That includes OLD last line type, NEW current line
 * type, non 0 value in BEG if STR contains first line bytes and EOL
 * end of line character if end of parsed line was reached.  Returns
 * non 0 value as long as something was parsed. */
int gmip_get(struct gmip *ps, char *str, size_t siz, FILE *fp);

/* Same as gmip_get except that STR will be filled with strings that
 * is already prepared to be printed in standard output with new line
 * characters at the end of each line and line type prefixes.  It
 * implements the simplest possible way of using gmip_get. */
int gmip_2stdout(struct gmip *ps, char *str, size_t siz, FILE *fp);

/* Same as gmip_get except that STR will be filled with strings
 * already parsed to Markdown. */
int gmip_2md(struct gmip *ps, char *str, size_t siz, FILE *fp);

/* Same as gmip_get except that STR will be filled with strings
 * already parsed to HTML. */
int gmip_2html(struct gmip *ps, char *str, size_t siz, FILE *fp);

#endif	/* GMIP_H */
#ifdef GMIP_IMPLEMENTATION
#include <assert.h>
#include <string.h>

int
gmip__prepend(char *dsc, size_t siz, char *src)
{
	size_t i;
	size_t dsc_len = strlen(dsc);
	size_t src_len = strlen(src);

	if (dsc_len + src_len >= siz) {
		return 0;
	}
	for (i = dsc_len; i; i--) {
		dsc[src_len+i] = dsc[i];
	}
	dsc[src_len+i] = dsc[i];
	for (i = 0; i < src_len; i++) {
		dsc[i] = src[i];
	}
	return 1;
}

int
gmip__append(char *dsc, size_t siz, char *src)
{
	size_t i;
	size_t dsc_len = strlen(dsc);
	size_t src_len = strlen(src);

	if (dsc_len + src_len >= siz) {
		return 0;
	}
	for (i = 0; i < src_len; i++) {
		dsc[dsc_len+i] = src[i];
	}
	dsc[dsc_len+i] = src[i];
	return 1;
}

int
gmip__fskip(FILE *fp, char *skip)
{
	int i, c;

	while ((c = fgetc(fp))) {
		for (i = 0; skip[i] != '\0'; i++) {
			if (c == skip[i])
				break;
		}
		if (skip[i] == '\0')
			break;
	}
	return c;
}

enum gmip_lt
gmip__lt(char *str, FILE *fp)
{
	size_t i = 0;		/* STR index */

	/* Skip empty lines with whitespaces and leading whitespaces
	 * in paragraphs. */
	while (1) {
		str[i] = gmip__fskip(fp, "\n");
		if (str[i] != ' ' && str[i] != '\t') {
			break;
		}
		str[i] = gmip__fskip(fp, " \t");
		if (str[i] != '\n') {
			str[++i] = '\0';
			return GMIP_P;
		}
	}
	str[++i] = '\0';

	/**/ if (str[i-1] == EOF) return GMIP_NUL;
	else if (str[i-1] == '>') return GMIP_Q;
	else if (str[i-1] == '*') return GMIP_LI;

	str[i] = fgetc(fp);
	str[++i] = '\0';
	if (str[i-1] == EOF || str[i-1] == '\n') return GMIP_P;

	/**/ if (!strcmp(str, "# "))  return GMIP_H1;
	else if (!strcmp(str, "#\t")) return GMIP_H1;
	else if (!strcmp(str, "=>"))  return GMIP_URL;

	str[i] = fgetc(fp);
	str[++i] = '\0';
	if (str[i-1] == EOF || str[i-1] == '\n') return GMIP_P;

	/**/ if (!strcmp(str, "## "))  return GMIP_H2;
	else if (!strcmp(str, "##\t")) return GMIP_H2;
	else if (!strcmp(str, "```"))  return GMIP_PRE;

	str[i] = fgetc(fp);
	str[++i] = '\0';
	if (str[i-1] == EOF || str[i-1] == '\n') return GMIP_P;

	/**/ if (!strcmp(str, "### "))  return GMIP_H3;
	else if (!strcmp(str, "###\t")) return GMIP_H3;

	return GMIP_P;
}

int
gmip_get(struct gmip *ps, char *str, size_t siz, FILE *fp)
{
	size_t i = 0;

	/* STR needs to fit at least 5 bytes to fit the longest
	 * possible line prefix with NUL terminator. */
	assert(siz > GMIP__MIN_PREF);

	str[0] = 0;
	ps->beg = 0;
	if (ps->eol) {
		if (ps->eol == EOF) {
			return 0; /* End parsing here */
		}
		ps->old = ps->new;
		ps->new = 0;
	}
	/* Get NEW line type. */
	if (!ps->new) {
		ps->beg = 1;
		/* When previous type was URL then next one is always
		 * DSC, no matter if it's empty or not.  Otherwise we
		 * search for new line type with gmip__lt. */
		if (ps->old == GMIP_URL) {
			ps->new = GMIP_DSC;
			if (ps->eol == '\n') {
				return 1;
			}
		} else {
			ps->new = gmip__lt(str, fp);
			/* In case of URL clear ps->url string value.
			 * In case of paragraph preserve characters
			 * that are already in STR. */
			if (ps->new == GMIP_URL) {
				ps->url[0] = 0;
			} else  if (ps->new == GMIP_P) {
				i = strlen(str);
			}
			/* End parsing on EOF or NUL. */
			if (str[i-1] == EOF || ps->new == GMIP_NUL) {
				ps->eol = EOF;
				str[0] = 0;
				return 0; /* End parsing here */
			}
		}
		ps->eol = 0;
		/* Skip whitespaces. */
		if ((str[i++] = gmip__fskip(fp, " \t")) == '\n') {
			if (ps->new == GMIP_PRE) {
				i = 0;
			} else {
				ps->eol = '\n';
				str[0] = 0;
				return 1;
			}
		}
	}
	/* Fill STR with line content as much as possible.  Mark end
	 * of line based of line kind. */
	siz--;			/* Make space ofr \0 */
	for (; i < siz; i++) {
		str[i] = fgetc(fp);
		/* Ignore '\r' character.  It appears at the end of
		 * response header line and makes printing broken. */
		if (str[i] == '\r') {
			str--;
			continue;
		}
		/* We reached EOF but let's not end here because STR
		 * migh still have some characters to print.  Just set
		 * ps->eol with EOF so we can end parsing on in next
		 * round. Otherwise if we are parsing PRE content
		 * avoid ending on new line and end only on ```.
		 * Otherwise always end line on new line character or
		 * in case of URL also end on space or tab. */
		if (str[i] == EOF) {
			ps->eol = EOF;
			break;
		} else if (ps->new == GMIP_PRE) {
			if (str[i] == '\n') {
				i++;
				break;
			}
			if (i == 2 && !strncmp(str, "```", 3)) {
				i = 0;
				ps->eol = '`';
				break;
			}
		} else if ((str[i] == '\n') ||
			   (ps->new == GMIP_URL &&
			    (str[i] == ' ' || str[i] == '\t'))) {
			ps->eol = str[i];
			break;
		}
	}
	/* Trim STR from spaces and tabs at the end. */
	if (ps->eol) {
		while (--i > 0 && (str[i] == ' ' || str[i] == '\t'));
		i++;
	}
	str[i] = 0;
	/* Sore URL for later usage. */
	if (ps->new == GMIP_URL) {
		/* Ignore error.  If for some reason URL is longer
		 * than it should be it will be cropped.  IDK what
		 * else I should do about it?  Log every too long URL
		 * to stdout? */
		gmip__append(ps->url, GMIP__URL_MAX, str);
	}
	return 1;
}

int
gmip_2stdout(struct gmip *ps, char *str, size_t siz, FILE *fp)
{
	int res;

	/* STR has to be long enough to fit longest stdout prefix. */
	assert(siz > GMIP__2STDOUT_PAD);
	res = gmip_get(ps, str, siz-GMIP__2STDOUT_PAD, fp);
	/* Print line prefix when line starts. */
	if (ps->beg) {
		/* Print last URL as DSC when DSC is empty. */
		if (ps->new == GMIP_DSC && strlen(str) == 0) {
			gmip__prepend(str, siz, ps->url);
		}
		/* Separate prifix from content with tab. */
		gmip__prepend(str, siz, "\t");
		/* Print prefix. */
		switch (ps->new) {
		case GMIP_NUL: break;
		case GMIP_H1:  gmip__prepend(str, siz, "h1");  break;
		case GMIP_H2:  gmip__prepend(str, siz, "h2");  break;
		case GMIP_H3:  gmip__prepend(str, siz, "h3");  break;
		case GMIP_P:   gmip__prepend(str, siz, "p");   break;
		case GMIP_URL: gmip__prepend(str, siz, "url"); break;
		case GMIP_DSC: gmip__prepend(str, siz, "dsc"); break;
		case GMIP_LI:  gmip__prepend(str, siz, "li");  break;
		case GMIP_Q:   gmip__prepend(str, siz, "q");   break;
		case GMIP_PRE: gmip__prepend(str, siz, "pre"); break;
		}
	}
	/* Put new line on Enf Of Line. */
	if (ps->eol) {
		gmip__append(str, siz, "\n");
	}
	return res;
}

int
gmip_2md(struct gmip *ps, char *str, size_t siz, FILE *fp)
{
	int res;

	/* STR has to be long enough to fit longest Markdown syntax
	 * characters. */
	assert(siz > GMIP__2MD_PAD);
	res = gmip_get(ps, str, siz-GMIP__2MD_PAD, fp);
	/* Avoid printing URL.  URL is printed on DSC from ps->url. */
	if (ps->new == GMIP_URL) {
		str[0] = 0;
	}
	/* Open line type. */
	if (ps->beg) {
		switch (ps->new) {
		case GMIP_NUL: break;
		case GMIP_H1:  gmip__prepend(str, siz, "# "); break;
		case GMIP_H2:  gmip__prepend(str, siz, "## "); break;
		case GMIP_H3:  gmip__prepend(str, siz, "### "); break;
		case GMIP_P:   break;
		case GMIP_URL: break;
		case GMIP_DSC: gmip__prepend(str, siz, "- ["); break;
		case GMIP_LI:  gmip__prepend(str, siz, "- "); break;
		case GMIP_Q:   gmip__prepend(str, siz, "> "); break;
		case GMIP_PRE: gmip__prepend(str, siz, "```\n"); break;
		}
	}
	/* Add extra empty line after block of links and block of list
	 * items.  By compering new to old line type we can detect
	 * changes in line blocks. */
	if ((ps->new != GMIP_URL && ps->old == GMIP_DSC) ||
	    (ps->new != GMIP_LI  && ps->old == GMIP_LI)) {
		gmip__prepend(str, siz, "\n");
	}
	if (ps->eol) {
		if (ps->new == GMIP_DSC) {
			if (strlen(str) <= 3) {
				gmip__append(str, siz, ps->url);
			}
			gmip__append(str, siz, "](");
			gmip__append(str, siz, ps->url);
			gmip__append(str, siz, ")\n");
		} else if (ps->new == GMIP_PRE) {
			gmip__append(str, siz, "```\n\n");
		} else if (ps->new != GMIP_URL) {
			gmip__append(str, siz, "\n");
			if (ps->new != GMIP_LI) {
				gmip__append(str, siz, "\n");
			}
		}
	}
	return res;
}

int
gmip_2html(struct gmip *ps, char *str, size_t siz, FILE *fp)
{
	int res;

	/* STR has to be long enough to fit longest HTML tag prefix
	 * and suffix. */
	assert(siz > GMIP__2HTML_PAD);
	res = gmip_get(ps, str, siz-GMIP__2HTML_PAD, fp);
	/* Open tag when line just started. */
	if (ps->beg) {
		/* Open line tag. */
		switch (ps->new) {
		case GMIP_NUL: break;
		case GMIP_H1:  gmip__prepend(str, siz, "<h1>"); break;
		case GMIP_H2:  gmip__prepend(str, siz, "<h2>"); break;
		case GMIP_H3:  gmip__prepend(str, siz, "<h3>"); break;
		case GMIP_P:   gmip__prepend(str, siz, "<p>"); break;
		case GMIP_URL: gmip__prepend(str, siz, "<li><a href=\""); break;
		case GMIP_DSC: if (!str[0]) gmip__prepend(str, siz, ps->url); break;
		case GMIP_LI:  gmip__prepend(str, siz, "<li>"); break;
		case GMIP_Q:   gmip__prepend(str, siz, "<blockquote>"); break;
		case GMIP_PRE: gmip__prepend(str, siz, "<pre>"); break;
		}
		/* Open tag groups. */
		if (ps->new == GMIP_URL && ps->old != GMIP_DSC) {
			gmip__prepend(str, siz, "<ol>\n"); /* Use <ol> for links */
		} else if (ps->new == GMIP_LI && ps->old != GMIP_LI) {
			gmip__prepend(str, siz, "<ul>\n");
		}
		/* Close tag groups. */
		if (ps->new != GMIP_URL && ps->old == GMIP_DSC) {
			gmip__prepend(str, siz, "</ol>\n");
		} else if (ps->new != GMIP_LI && ps->old == GMIP_LI) {
			gmip__prepend(str, siz, "</ul>\n");
		}
	}
	/* Close tag on End Of Line. */
	if (ps->eol && ps->eol != EOF) {
		switch (ps->new) {
		case GMIP_NUL: break;
		case GMIP_H1:  gmip__append(str, siz, "</h1>"); break;
		case GMIP_H2:  gmip__append(str, siz, "</h2>"); break;
		case GMIP_H3:  gmip__append(str, siz, "</h3>"); break;
		case GMIP_P:   gmip__append(str, siz, "</p>"); break;
		case GMIP_URL: gmip__append(str, siz, "\">"); break;
		case GMIP_DSC: gmip__append(str, siz, "</a></li>"); break;
		case GMIP_LI:  gmip__append(str, siz, "</li>"); break;
		case GMIP_Q:   gmip__append(str, siz, "</blockquote>"); break;
		case GMIP_PRE: gmip__append(str, siz, "</pre>"); break;
		}
		/* Avoid adding new line after URL because
		 * next one will be DSC and we wont to have
		 * them in the same line. */
		if (ps->new != GMIP_URL) {
			gmip__append(str, siz, "\n");
		}
	}
	return res;
}

#endif	/* GMIP_IMPLEMENTATION */
