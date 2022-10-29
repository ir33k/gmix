/* Parse text/gemini text format to HTML syntax. */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIP_IMPLEMENTATION
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
		die("Parse Gemeni text to HTML.\n\n"
		    "usage: %s [-h] [file]\n\n"
		    "\t-h\tPrint this usage help message.\n"
		    "\tfile\tFile to parse, use stdin by default.\n",
		    argv[0]);
	}
	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}
	while (1) {
		gmip(&ps, str, BSIZ, fp);
		/* Open tag when line just started. */
		if (ps.beg) {
			/* Open or close tag groups. */
			if (ps.new == GMIP_URL && ps.old != GMIP_DSC) {
				printf("<ul class=\"links\">\n");
			} else if (ps.new == GMIP_LI && ps.old != GMIP_LI) {
				printf("<ul>\n");
			} else if (ps.new != GMIP_LI && ps.old == GMIP_LI) {
				printf("</ul>\n");
			} else if (ps.new != GMIP_URL && ps.old == GMIP_DSC) {
				printf("</ul>\n");
			}
			/* Open line tag. */
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
			if (ps.eol == EOF) {
				break;
			}
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
			/* Avoid adding new line after URL because
			 * next one will be DSC and we wont to have
			 * them in the same line. */
			if (ps.new != GMIP_URL) {
				putchar('\n');
			}
		}
	}
	if (fp != stdin && fclose(fp) == EOF) {
		die("fclose:");
	}
	return 0;
}
