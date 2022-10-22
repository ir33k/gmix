#define _GNU_SOURCE
#include <stdio.h>
#include "walter.h"
#include "parse.h"

TEST("parse")
{
	Parse state = PARSE_NUL;
	char buf[8];
	FILE *fp;

	char *in = "20 text/gemini128\n\n"
		"# Title\n"
		"## Sub title\n"
		"### Sub sub title\n"
		"\n"
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
		"\n"
		"End Of Line\n";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "20 text");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "/gemini");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "128");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_H1));
	STR_EQ(&buf[0], "Title");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_H2));
	STR_EQ(&buf[0], "Sub tit");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_H2));
	STR_EQ(&buf[0], "le");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_H3));
	STR_EQ(&buf[0], "Sub sub");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_H3));
	STR_EQ(&buf[0], " title");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "Regular");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], " text");

	/* URL 1 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "host1.n");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "ame/pat");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "h");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_DSC));
	STR_EQ(&buf[0], "link de");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_DSC));
	STR_EQ(&buf[0], "scripti");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_DSC));
	STR_EQ(&buf[0], "on 1");

	/* URL 2 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "host2.n");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "ame/pat");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "h");

	/* URL 3 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "host3.n");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "ame/pat");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "h");

	/* URL 4 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "host4.n");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "ame/pat");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_URL));
	STR_EQ(&buf[0], "h");

	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_DSC));
	STR_EQ(&buf[0], "link de");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_DSC));
	STR_EQ(&buf[0], "scripti");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_DSC));
	STR_EQ(&buf[0], "on 4");

	/* LI 1 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "First l");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "ist ite");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "m");

	/* LI 2 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "Second ");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "list it");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "em");

	/* LI 3 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "Third l");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "ist ite");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_ULI));
	STR_EQ(&buf[0], "m");

	/* Paragraph 2 */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "Regular");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], " text 2");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "");

	/* Quote */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_Q));
	STR_EQ(&buf[0], "Quote t");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_Q));
	STR_EQ(&buf[0], "ext");

	/* Preformatted text */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_PRE));
	STR_EQ(&buf[0], "pre hea");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_PRE));
	STR_EQ(&buf[0], "der\t\n");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_PRE));
	STR_EQ(&buf[0], "    pre");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_PRE));
	STR_EQ(&buf[0], " conten");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_PRE));
	STR_EQ(&buf[0], "t   \n");

	/* TODO(irek): I don't like this list empty line in pre text.
	 * This will always happen with current implementation. */
	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_PRE));
	STR_EQ(&buf[0], "");

	/* Last paragraph that don't ends with \n. */
	OK((state = parse(state, buf, 8, fp)));
	OK( (state & PARSE_BEG));
	OK(!(state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "End Of ");

	OK((state = parse(state, buf, 8, fp)));
	OK(!(state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "Line");

	/* EOF */
	OK((state = parse(state, buf, 8, fp)) == PARSE_NUL);
	STR_EQ(&buf[0], "");
}

TEST("Edge case for EOF")
{
	Parse state = PARSE_NUL;
	char buf[16];
	FILE *fp;

	char *in = "Single line";

	if ((fp = fmemopen(in, strlen(in), "rb")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	OK((state = parse(state, buf, 16, fp)));
	OK( (state & PARSE_BEG));
	OK( (state & PARSE_END));
	OK( (state & PARSE_P));
	STR_EQ(&buf[0], "Single line");

	OK((state = parse(state, buf, 16, fp)) == PARSE_NUL);
	STR_EQ(&buf[0], "");
}
