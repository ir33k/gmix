/* Parse text/gemini text format to Markdown syntax. */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIP_IMPLEMENTATION
#include "gmip.h"

#define URLSIZ  1024+1		/* Buffer size that can hold URL */

/* If VAL is smaller than MIN, return MIN.  When it's bigger than MAX,
 * return MAX.  Otherwise return original VAL.*/
int clamp(int min, int val, int max);

int
clamp(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

int
main(int argc, char **argv)
{
	struct gmip ps = {0};
	char url[URLSIZ];
	char str[BUFSIZ];
	FILE *fp = stdin;

	if (getopt(argc, argv, "h") != -1) {
		die("Parse Gemeni text to Markdown.\n\n"
		    "usage: %s [-h] [file]\n\n"
		    "\t-h\tPrint this usage help message.\n"
		    "\tfile\tFile to parse, use stdin by default.\n",
		    argv[0]);
	}
	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}
	while (gmip(&ps, str, BUFSIZ, fp)) {
		/* Add extra empty line after block of links and block
		 * of list items. */
		if ((ps.new != GMIP_URL && ps.old == GMIP_DSC) ||
		    (ps.new != GMIP_LI  && ps.old == GMIP_LI)) {
			putchar('\n');
		}
		/* Open line type. */
		if (ps.beg) {
			switch (ps.new) {
			case GMIP_NUL: break;
			case GMIP_H1:  printf("# "); break;
			case GMIP_H2:  printf("## "); break;
			case GMIP_H3:  printf("### "); break;
			case GMIP_P:   break;
			case GMIP_URL: url[0] = '\0'; break;
			case GMIP_DSC: printf("- ["); break;
			case GMIP_LI:  printf("- "); break;
			case GMIP_Q:   printf("> "); break;
			case GMIP_PRE: printf("```\n"); break;
			}
		}
		/* Print line content.  In case of URL store string in
		 * separate URL buffer to print it after DSC. */
		if (ps.new == GMIP_URL) {
			strncat(url, str, clamp(0, strlen(str), URLSIZ - strlen(url) - 1));
		} else {
			fputs(str, stdout);
		}
		if (ps.eol) {
			if (ps.new == GMIP_DSC) {
				printf("%s](%s)\n", strlen(str) ? "" : url, url);
			} else if (ps.new == GMIP_PRE) {
				printf("```\n\n");
			} else if (ps.new != GMIP_URL) {
				putchar('\n');
				if (ps.new != GMIP_LI) {
					putchar('\n');
				}
			}
		}
	}
	if (fp != stdin && fclose(fp) == EOF) {
		die("fclose:");
	}
	return 0;
}
