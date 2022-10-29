/* Parse text/gemini text format so lines are prefixed with type. */

#include <stdio.h>
#include <getopt.h>
#define UTIL_IMPLEMENTATION
#include "util.h"
#define GMIP_IMPLEMENTATION
#include "gmip.h"

/* Small buffer size is used to test how well parse.h deals with lines
 * delivered in multiple parts. */

#define BSIZ	8		/* Buffer size */

int
main(int argc, char **argv)
{
	struct gmip ps = {0};
	char str[BSIZ];
	FILE *fp = stdin;

	if (getopt(argc, argv, "h") != -1) {
		printf("GMI Parse 2 text - Parse Gemeni text.\n\n"
		       "usage: %s [-h] [file]\n\n"
		       "\t-h\tPrint this usage help message.\n"
		       "\tfile\tFile to parse, use stdin by default.\n\n",
		       argv[0]);
		return 1;
	}
	if (argc > 1) {
		if ((fp = fopen(argv[1], "rb")) == NULL)
			die("fopen:");
	}
	while (gmip(&ps, str, BSIZ, fp)) {
		if (ps.beg) {
			switch (ps.new) {
			case GMIP_NUL: break;
			case GMIP_H1:  printf("h1");  break;
			case GMIP_H2:  printf("h2");  break;
			case GMIP_H3:  printf("h3");  break;
			case GMIP_P:   printf("p");   break;
			case GMIP_URL: printf("url"); break;
			case GMIP_DSC: printf("dsc"); break;
			case GMIP_LI:  printf("li");  break;
			case GMIP_Q:   printf("q");   break;
			case GMIP_PRE: printf("pre"); break;
			}
			putchar('\t');
		}
		fputs(str, stdout);
		if (ps.eol) {
			putchar('\n');
		}
	}
	if (fp != stdin && fclose(fp) == EOF) {
		die("fclose:");
	}
	return 0;
}
