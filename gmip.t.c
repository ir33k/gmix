#define _GNU_SOURCE
#include <stdio.h>
#include "walter.h"
#include "gmip.h"

TEST("All general parser cases")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[8];
	FILE *fp;

	char *in = "20 text/gemini128\n\n"
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
		"* First list item   \n"
		"* Second list item\n"
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
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "20 text");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "/gemini");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "128");

	/* H1 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_H1);
	OK(ps.beg == 1);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "Title");

	/* H2 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_H1);
	OK(ps.new == GMIP_H2);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Sub tit");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_H1);
	OK(ps.new == GMIP_H2);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "le");

	/* H3 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_H2);
	OK(ps.new == GMIP_H3);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Sub sub");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_H2);
	OK(ps.new == GMIP_H3);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], " title");

	/* P 2 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_H3);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Regular");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_H3);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], " text");

	/* URL 1 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "host1.n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "ame/pat");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == ' ');
	STR_EQ(&buf[0], "h");

	/* DSC 1 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "link de");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "scripti");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "on 1");

	/* URL 2 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "host2.n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "ame/pat");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == ' ');
	STR_EQ(&buf[0], "h");

	/* DSC 2 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "");

	/* URL 3 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "host3.n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "ame/pat");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "h");

	/* DSC 3 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "");

	/* URL 4 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "host4.n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "ame/pat");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_URL);
	OK(ps.beg == 0);
	OK(ps.eol == ' ');
	STR_EQ(&buf[0], "h");

	/* DSC 4 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "link de");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "scripti");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_URL);
	OK(ps.new == GMIP_DSC);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "on 4");

	/* LI 1 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "First l");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "ist ite");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_DSC);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "m");

	/* LI 2 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Second ");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "list it");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "em");

	/* LI 3 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Third l");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "ist ite");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_LI);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "m");

	/* P 3 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Regular");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], " text 2");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_LI);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "");

	/* Quote */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_Q);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "Quote t");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_Q);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "ext");

	/* PRE 1 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "pre hea");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "der\t\n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "    pre");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], " conten");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "t   \n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_Q);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == '`');
	STR_EQ(&buf[0], "");

	/* PRE 2 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "    pre");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], " conten");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "t   \n");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_PRE);
	OK(ps.beg == 0);
	OK(ps.eol == '`');
	STR_EQ(&buf[0], "");

	/* P 4 */
	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "End Of ");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_PRE);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == '\n');
	STR_EQ(&buf[0], "Line");

	/* EOF */
	OK(!gmip(&ps, buf, 8, fp));
	OK(ps.old == GMIP_P);
	OK(ps.new == GMIP_NUL);
	OK(ps.beg == 1);
	OK(ps.eol == EOF);
	STR_EQ(&buf[0], "");
}

TEST("EOF paragraph without last new line character")
{
	struct gmip ps = {0}; /* Parse State */
	char buf[8];
	FILE *fp;

	char *in = "End Of Line";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 1);
	OK(ps.eol == 0);
	STR_EQ(&buf[0], "End Of ");

	OK(gmip(&ps, buf, 8, fp));
	OK(ps.old == 0);
	OK(ps.new == GMIP_P);
	OK(ps.beg == 0);
	OK(ps.eol == EOF);
	STR_EQ(&buf[0], "Line");

	OK(!gmip(&ps, buf, 8, fp));
}
