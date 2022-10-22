#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "parse.h"

int
parse__fskip(FILE *fp, char *skip)
{
	size_t i;
	char c;

	while ((c = fgetc(fp))) {
		for (i = 0; skip[i] != '\0'; i++) {
			if (c == skip[i])
				break;
		}
		if (skip[i] == '\0')
			break;
	}
	/* Go back one character because at this point FP position is
	 * set to position of second character that is not in SKIP. */
	return c == EOF ? 0 : fseek(fp, -1, SEEK_CUR);
}

Parse
parse__kind(Parse old, FILE *fp)
{
	size_t i = 0;		/* STR index */
	char str[5];

	/* Special case for link desciption which does not start with
	 * new line but after URL and it's optional link line part.*/
	if (old & PARSE_URL) {
		fseek(fp, -1, SEEK_CUR);
		parse__fskip(fp, " \t");
		str[i] = fgetc(fp);
		if (str[i] == EOF) {
			return PARSE_NUL;
		}
		if (str[i] != '\n') {
			fseek(fp, -1, SEEK_CUR);
			return PARSE_DSC;
		}
	}

	parse__fskip(fp, " \t\n");
	str[i] = fgetc(fp);

	/**/ if (str[i] == EOF) return PARSE_NUL;
	else if (str[i] == '>') return PARSE_Q;
	else if (str[i] == '*') return PARSE_ULI;

	str[++i] = fgetc(fp);
	if (str[i] == EOF || str[i] == '\n') {
		fseek(fp, i*-1, SEEK_CUR);
		return PARSE_P;
	}
	str[++i] = '\0';
	
	/**/ if (!strcmp(str, "# "))  return PARSE_H1;
	else if (!strcmp(str, "#\t")) return PARSE_H1;
	else if (!strcmp(str, "=>"))  return PARSE_URL;

	str[i] = fgetc(fp);
	if (str[i] == EOF || str[i] == '\n') {
		fseek(fp, i*-1, SEEK_CUR);
		return PARSE_P;
	}
	str[++i] = '\0';

	/**/ if (!strcmp(str, "## "))  return PARSE_H2;
	else if (!strcmp(str, "##\t")) return PARSE_H2;
	else if (!strcmp(str, "```"))  return PARSE_PRE;

	str[i] = fgetc(fp);
	if (str[i] == EOF || str[i] == '\n') {
		fseek(fp, i*-1, SEEK_CUR);
		return PARSE_P;
	}
	str[++i] = '\0';

	/**/ if (!strcmp(str, "### "))  return PARSE_H3;
	else if (!strcmp(str, "###\t")) return PARSE_H3;

	fseek(fp, i*-1, SEEK_CUR);
	return PARSE_P;
}

Parse
parse(Parse old, char *str, size_t siz, FILE *fp)
{
	size_t i;
	Parse res = 0;

	/* Get new line kind when OLD parse state indicated that last
	 * line was fully parsed or it's first parse (NUL).  Else keep
	 * old state line kind without line info. */
	if (old & PARSE_END || old == PARSE_NUL) {
		res = parse__kind(old, fp);
		if (res == PARSE_NUL) {
			str[0] = '\0';
			return res;
		}
		res |= PARSE_BEG;
		parse__fskip(fp, " \t");
	} else {
		res = old;
		if (res & PARSE_BEG) res ^= PARSE_BEG;
		if (res & PARSE_END) res ^= PARSE_END;
	}
	if (res == PARSE_NUL) {
		str[0] = '\0';
		return res;
	}
	/* Fill STR with line content as much as possible.  Mark end
	 * of line based of line kind. */
	siz--;			/* Make space for '\0' */
	for (i = 0; i < siz; i++) {
		str[i] = fgetc(fp);

		if (str[i] == EOF) {
			res |= PARSE_END;
			break;
		} else if (res & PARSE_PRE) {
			if (str[i] == '\n') {
				i++;
				break;
			}
			if (i == 2 && !strncmp(str, "```", 3)) {
				i = 0;
				res |= PARSE_END;
				break;
			}
		} else if ((str[i] == '\n') ||
			   (res & PARSE_URL &&
			    (str[i] == ' ' || str[i] == '\t'))) {
			res |= PARSE_END;
			break;
		}
	}
	/* Trim STR from spaces and tabs at the end. */
	if (res & PARSE_END) {
		while (--i > 0 && (str[i] == ' ' || str[i] == '\t'));
		i++;
	}
	str[i] = '\0';
	return res;
}
