#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "gmip.h"

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

	/* Note that EOF can occur in the at any moment while parsing
	 * prefix.  In that case GMIP_P is returned because none other
	 * line type is valid and we can have few last characters
	 * before EOF that are shorter that some line prefixes. */

	/**/ if (str[i-1] == EOF) return GMIP_P;
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
		if (str[i-1] == EOF) {
			ps->eol = EOF;
			str[0] = 0;
			return 0;
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
