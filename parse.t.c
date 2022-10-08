#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include "walter.h"
#include "parse.h"

/* This test is very simple and ignore a lot of cases but it's already
 * something.  That helps me while working on parse.h lib so I don't
 * have to manually test everything myself. */
TEST("parse")
{
	/* Parse STATE with initial PARSE_NUL value. */
	Parse state = 0;
	char line[BUFSIZ] = {0};
	FILE *fp;

	/* Input string for parser. */
	char *in = "20 text/gemini128\n\n"
		"# Main title\n"
		"## Sub title\n"
		"### Sub sub title\n\n"
		"Regular text\n"
		"\n"
		"=> host1.name/path link description 1\n"
		"=> host2.name/path link description 2\n"
		"\n"
		"* First list item\n"
		"* Second list item\n"
		"* Third list item\n"
		"\n"
		"Regular text 2\n"
		"\n"
		"> Quote text\n"
		"\n"
		"``` pre header\n"
		"    pre content\n"
		"```\n"
		"\n";

	if ((fp = fmemopen(in, strlen(in), "r")) == NULL)
		ASSERT(0, "ERR: fmemopen");

	/* Not thet those assertion works only because BUFSIZ is big
	 * enough to get whole line on each parse call. */
	OK(state == PARSE_NUL);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_P);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_H1);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_H2);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_H3);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_P);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_URI);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_URI);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_LI);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_LI);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_LI);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_P);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_Q);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_PRE);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_PRE);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) & PARSE_BR);
	OK(state & PARSE_BEG && state & PARSE_END);
	OK((state = parse(state, line, BUFSIZ, fp)) == PARSE_NUL);
}

TODO("parse_clean")
{
}
