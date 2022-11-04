#define _GNU_SOURCE		/* Needed for fmemopen */
#include <stdio.h>
#include "walter.h"
#define GMIP_IMPLEMENTATION
#include "gmip.h"

TEST("gmip__prepend")
{
	char buf[28], *bp = buf;

	buf[0] = 0;
	strcat(bp, "some regular text");

	OK(gmip__prepend(bp, 28, "prefix: "));
	STR_EQ(bp, "prefix: some regular text");
	OK(!gmip__prepend(bp, 28, "long prefix"));
	STR_EQ(bp, "prefix: some regular text");
}

TEST("gmip__append")
{
	char buf[28], *bp = buf;

	buf[0] = 0;
	strcat(bp, "some regular text");

	OK(gmip__append(bp, 28, " :suffix"));
	STR_EQ(bp, "some regular text :suffix");
	OK(!gmip__append(bp, 28, "long suffix"));
	STR_EQ(bp, "some regular text :suffix");
}

TEST("gmip_get: general cases")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[8], *bp = buf;
	FILE *fp;

	char *in = "20 text/gemini128\r\n\n"
		"#  \t\t  Title\n"
		"##\t\t Sub title\n"
		"### Sub sub title\n"
		"\n"
		"  \t\t  \n"
		"\n"
		" \t\t  Regular text\n"
		"\n"
		"=> host1.name/path link description 1 \t\n"
		"=> host2.name/path  \t\t  \n"
		"=> host3.name/path\n"
		"=> host4.name/path 	 link description 4\n"
		"\n"
		"*First list item   \n"
		"*\t\t  Second list item\n"
		"* Third list item\n"
		"\n"
		"Regular text 2\n"
		"\n"
		"> Quote text\t  \n"
		"\n"
		"``` pre header\t\n"
		"    pre content   \n"
		"```\n"
		"```\n"
		"    pre content   \n"
		"```\n"
		"\n"
		"End Of Line\n";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	/* P 1 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "20 text");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "/gemini");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "128");

	/* H1 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_H1);
	OK(ps.beg == 1);
	OK(ps.eol == '\n');
	STR_EQ(bp, "Title");

	/* H2 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_H1);
	OK(ps.new == GMIP_H2);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Sub tit");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_H1);
	OK(ps.new == GMIP_H2);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "le");

	/* H3 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_H2);
	OK(ps.new == GMIP_H3);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Sub sub");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_H2);
	OK(ps.new == GMIP_H3);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, " title");

	/* P 2 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_H3);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Regular");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_H3);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, " text");

	/* URL 1 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "host1.n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "ame/pat");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == ' ');
	STR_EQ(bp, "h");

	/* DSC 1 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "link de");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "scripti");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "on 1");

	/* URL 2 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "host2.n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "ame/pat");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == ' ');
	STR_EQ(bp, "h");

	/* DSC 2 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == '\n');
	STR_EQ(bp, "");

	/* URL 3 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "host3.n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "ame/pat");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "h");

	/* DSC 3 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == '\n');
	STR_EQ(bp, "");

	/* URL 4 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "host4.n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "ame/pat");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == ' ');
	STR_EQ(bp, "h");

	/* DSC 4 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "link de");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "scripti");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "on 4");

	/* LI 1 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "First l");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "ist ite");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "m");

	/* LI 2 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Second ");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "list it");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "em");

	/* LI 3 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Third l");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "ist ite");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "m");

	/* P 3 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Regular");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, " text 2");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "");

	/* Quote */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_Q);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "Quote t");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_Q);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "ext");

	/* PRE 1 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "pre hea");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "der\t\n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "    pre");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, " conten");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "t   \n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == '`');
	STR_EQ(bp, "");

	/* PRE 2 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "    pre");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, " conten");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(bp, "t   \n");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == '`');
	STR_EQ(bp, "");

	/* P 4 */
	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "End Of ");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(bp, "Line");

	/* EOF */
	OK(!gmip_get(&ps, bp, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_NUL);
	OK(ps.beg == 1);
	OK(ps.eol == EOF);
	STR_EQ(bp, "");
}

TEST("gmip_get: EOF paragraph without last new line character")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[8], *bp = buf;
	FILE *fp;

	char *in = "End Of Line";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(bp, "End Of ");

	OK(gmip_get(&ps, bp, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == EOF);
	STR_EQ(bp, "Line");

	OK(!gmip_get(&ps, bp, 8, fp));
}

TEST("gmip_2stdout")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[256], *bp = buf;
	FILE *fp;

	char *in = "20 text/gemini128\r\n\n"
		"#  \t\t  Title\n"
		"##\t\t Sub title\n"
		"### Sub sub title\n"
		"\n"
		"  \t\t  \n"
		"\n"
		" \t\t  Regular text\n"
		"\n"
		"=> host1.name/path link description 1 \t\n"
		"=> host2.name/path  \t\t  \n"
		"=> host3.name/path\n"
		"=> host4.name/path 	 link description 4\n"
		"\n"
		"*First list item   \n"
		"*\t\t  Second list item\n"
		"* Third list item\n"
		"\n"
		"Regular text 2\n"
		"\n"
		"> Quote text\t  \n"
		"\n"
		"``` pre header\t\n"
		"    pre content   \n"
		"```\n"
		"```\n"
		"    pre content   \n"
		"```\n"
		"\n"
		"End Of Line\n";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "p\t20 text/gemini128\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "h1\tTitle\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "h2\tSub title\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "h3\tSub sub title\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "p\tRegular text\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "url\thost1.name/path\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "dsc\tlink description 1\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "url\thost2.name/path\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "dsc\thost2.name/path\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "url\thost3.name/path\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "dsc\thost3.name/path\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "url\thost4.name/path\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "dsc\tlink description 4\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "li\tFirst list item\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "li\tSecond list item\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "li\tThird list item\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "p\tRegular text 2\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "q\tQuote text\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "pre\tpre header\t\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "    pre content   \n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "pre\t    pre content   \n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "\n");
	OK(gmip_2stdout(&ps, bp, 256, fp));
	STR_EQ(bp, "p\tEnd Of Line\n");
}

TEST("gmip_2md")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[256], *bp = buf;
	FILE *fp;

	char *in = "20 text/gemini128\r\n\n"
		"#  \t\t  Title\n"
		"##\t\t Sub title\n"
		"### Sub sub title\n"
		"\n"
		"  \t\t  \n"
		"\n"
		" \t\t  Regular text\n"
		"\n"
		"=> host1.name/path link description 1 \t\n"
		"=> host2.name/path  \t\t  \n"
		"=> host3.name/path\n"
		"=> host4.name/path 	 link description 4\n"
		"\n"
		"*First list item   \n"
		"*\t\t  Second list item\n"
		"* Third list item\n"
		"\n"
		"Regular text 2\n"
		"\n"
		"> Quote text\t  \n"
		"\n"
		"``` pre header\t\n"
		"    pre content   \n"
		"```\n"
		"```\n"
		"    pre content   \n"
		"```\n"
		"\n"
		"End Of Line\n";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "20 text/gemini128\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "# Title\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "## Sub title\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "### Sub sub title\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "Regular text\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "- [link description 1](host1.name/path)\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "- [host2.name/path](host2.name/path)\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "- [host3.name/path](host3.name/path)\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "- [link description 4](host4.name/path)\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "\n- First list item\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "- Second list item\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "- Third list item\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "\nRegular text 2\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "> Quote text\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "```\npre header\t\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "    pre content   \n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "```\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "```\n    pre content   \n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "```\n\n");
	OK(gmip_2md(&ps, bp, 256, fp));
	STR_EQ(bp, "End Of Line\n\n");
}

TEST("gmip_2html")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[256], *bp = buf;
	FILE *fp;

	char *in = "20 text/gemini128\r\n\n"
		"#  \t\t  Title\n"
		"##\t\t Sub title\n"
		"### Sub sub title\n"
		"\n"
		"  \t\t  \n"
		"\n"
		" \t\t  Regular text\n"
		"\n"
		"=> host1.name/path link description 1 \t\n"
		"=> host2.name/path  \t\t  \n"
		"=> host3.name/path\n"
		"=> host4.name/path 	 link description 4\n"
		"\n"
		"*First list item   \n"
		"*\t\t  Second list item\n"
		"* Third list item\n"
		"\n"
		"Regular text 2\n"
		"\n"
		"> Quote text\t  \n"
		"\n"
		"``` pre header\t\n"
		"    pre content   \n"
		"```\n"
		"```\n"
		"    pre content   \n"
		"```\n"
		"\n"
		"End Of Line\n";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<p>20 text/gemini128</p>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<h1>Title</h1>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<h2>Sub title</h2>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<h3>Sub sub title</h3>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<p>Regular text</p>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<ol>\n<li><a href=\"host1.name/path\">");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "link description 1</a></li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<li><a href=\"host2.name/path\">");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "host2.name/path</a></li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<li><a href=\"host3.name/path\">");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "host3.name/path</a></li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<li><a href=\"host4.name/path\">");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "link description 4</a></li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "</ol>\n<ul>\n<li>First list item</li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<li>Second list item</li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<li>Third list item</li>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "</ul>\n<p>Regular text 2</p>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<blockquote>Quote text</blockquote>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<pre>pre header\t\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "    pre content   \n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "</pre>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<pre>    pre content   \n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "</pre>\n");
	OK(gmip_2html(&ps, bp, 256, fp));
	STR_EQ(bp, "<p>End Of Line</p>\n");
}
