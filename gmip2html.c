/* Parse text/gemini text format to HTML syntax. */

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "util.h"
#include "gmip.h"

/* It's probably better to use default BUFSIZ value defined in stdio.h
 * for buffer size which on modern machines is much higher but that
 * would result in parsing each line as a whole and I want to do it in
 * small parts to test parser capabilities and catch mistakes. */

#define BSIZ    32		/* Buffer size */
#define URLSIZ  1024+1		/* Buffer size that can hold URL */
#define INDENT  "\t"		/* Indentation for HTML tags */

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
	char str[BSIZ];
	FILE *fp = stdin;	/* Read from stdin by default */

	if (getopt(argc, argv, "h") != -1) {
		printf("GMI parse 2 HTML - Parse Gemeni text to HTML.\n\n"
		       "usage: %s [-h] [file]\n\n"
		       "\t-h\tPrint this usage help message.\n"
		       "\tfile\tFile to parse, use stdin by default.\n\n",
		       argv[0]);
		return 1;
	}
	/* TODO(irek): Make it possible to parse multiple files? */
	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}
	while (gmip(&ps, str, BSIZ, fp)) {
		/* Open or close tag groups when old and new status
		 * differs between parsing iterations. */
		if (ps.new == GMIP_URL && ps.old != GMIP_DSC) {
			printf("<ul class=\"links\">\n");
		} else if (ps.new == GMIP_LI && ps.old != GMIP_LI) {
			printf("<ul>\n");
		} else if (ps.new != GMIP_LI && ps.old == GMIP_LI) {
			printf("</ul>\n");
		} else if (ps.new != GMIP_URL && ps.old == GMIP_DSC) {
			printf("</ul>\n");
		}
		/* Open tag. */
		if (ps.beg) {
			switch (ps.new) {
			case GMIP_NUL: break;
			case GMIP_H1:  printf("<h1>"); break;
			case GMIP_H2:  printf("<h2>"); break;
			case GMIP_H3:  printf("<h3>"); break;
			case GMIP_P:   printf("<p>"); break;
			case GMIP_URL: printf(INDENT "<li><a href=\""); url[0] = '\0'; break;
			case GMIP_DSC: if (!str[0]) printf("%s", url); break;
			case GMIP_LI:  printf(INDENT "<li>"); break;
			case GMIP_Q:   printf("<blockquote>"); break;
			case GMIP_PRE: printf("<pre>"); break;
			}
		}
		/* Remember last URL so it's possible to print it
		 * again if optional DSC for link is not provided. */
		if (ps.new == GMIP_URL) {
			strncat(url, str, clamp(0, strlen(str), URLSIZ - strlen(url) - 1));
		}
		/* Print content of buf. */
		fputs(str, stdout);
		/* Close tag on End Of Line. */
		if (ps.eol) {
			switch (ps.new) {
			case GMIP_NUL: break;
			case GMIP_H1:  printf("</h1>"); break;
			case GMIP_H2:  printf("</h2>"); break;
			case GMIP_H3:  printf("</h3>"); break;
			case GMIP_P:   printf("</p>"); break;
			case GMIP_URL: printf("\">"); break;
			case GMIP_DSC: printf("</a></li>"); break;
			case GMIP_LI:  printf("</li>"); break;
			case GMIP_Q:   printf("</blockquote>"); break;
			case GMIP_PRE: printf("</pre>"); break;
			}
			if (ps.new != GMIP_URL) {
				putchar('\n');
			}
		}
	}
	/* TODO(irek): Blocks of <ul> for links and lists are not
	 * closed after EOF.  This does not make HTML broken but it
	 * would be nicer if blocks are always closed. */
	if (fp != stdin && fclose(fp) == EOF) {
		die("fclose:");
	}
	return 0;
}
