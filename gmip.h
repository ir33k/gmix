/* Parser Gemini text/gemini format.  Allows to easily go through
 * stream filling given buffer peace by peace along with struct gmip
 * that will tell what kind of line is it and at which part of line
 * parsing we are.  For simplest example see gmip.c program. */

#ifndef GMIP_H
#define GMIP_H
#include <stdio.h>

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
 * OLD and NEW are previous and current Line Type.  BEG is non 0 when
 * NEW line just started and EOL is last line end character which is
 * non 0 when end of line of current NEW type was reached. */
struct gmip {
	enum gmip_lt old, new;
	int beg;
	char eol;
};

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
int gmip(struct gmip *ps, char *str, size_t siz, FILE *fp);

#endif	/* GMIP_H */
#ifdef GMIP_IMPLEMENTATION
#include <assert.h>
#include <string.h>

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
gmip(struct gmip *ps, char *str, size_t siz, FILE *fp)
{
	size_t i = 0;

	/* STR needs to fit at least 5 bytes to fit the longest
	 * possible line prefix with NUL terminator. */
	assert(siz > 5);

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
		}
		ps->eol = 0;
		/* In case of paragraph there are already some
		 * characters in STR that we wan't to preserve. */
		if (ps->new == GMIP_P) {
			i = strlen(str);
		}
		/* End parsing on EOF or NUL (those are the same). */
		if (str[i-1] == EOF || ps->new == GMIP_NUL) {
			ps->eol = EOF;
			str[0] = 0;
			return 0; /* End parsing here */
		}
		/* Skip whitespaces  */
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
	return 1;
}

#endif	/* GMIP_IMPLEMENTATION */
