/* Single header library for creating unit tests programs.
 *
 * Can be used (included) only in one test program because it relays
 * on single global tests state and IT macros line numbers.  You can
 * define only 64 tests by default.  Predefining TESTMAX value before
 * "test.h" is included to change that.  Variables, functions and
 * macros not mentioned in usage example should not be used.
 *
 * Usage test program:
 *
 *	// file: program.t.c
 *	#define TESTMAX 32              // Define to handle more tests
 *	#include "test.h"               // Get test lib
 *
 *	IT("Should pass")               // Define test with test cases
 *	{
 *		// Defined test will end on first failing test case
 *		ASSERT(boolean, "custom message on fail");
 *		OK(boolean);
 *		NUM_EQ(number1, number2);
 *		STR_EQ(str1, str2);
 *		BUF_EQ(buf1, buf2, size);
 *
 *		// Helper functions that returns boolean
 *		OK(test_str_eq(str1, str2));
 *		OK(test_buf_eq(str1, str2, size));
 *
 *		FAIL("fail message");   // End test here with fail
 *		PASS();                 // End test here with success
 *	}
 *
 *	IT("Another test 1") { ... }    // Define as many as TESTMAX
 *	IT("Another test 2") { ... }
 *
 *	// Note that there is not "main" function
 *
 * Compile and run "program" tests with:
 * 
 *	$ cc -o program.t program.t.c && ./program.t
 */
#ifdef TEST_H_
/* Error for those who don't read documentation and/or comments. */
#error "test.h library can't be used in multiple files"
#else
#define TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef TESTMAX			/* Predefined for more tests */
#define TESTMAX    64		/* Maximum number of tests */
#endif

#define IT____(_msg, id, l)                                     \
	void test__body##id(void);                              \
	/* This function will be run before "main" function.  It's the
	 * heart, main hack, my own precious ring of power that makes
	 * this test lib possible. */                           \
	void test__##id(void) __attribute__ ((constructor));    \
	void test__##id(void) {                                 \
		test__.msg[test__.all] = _msg;                  \
		test__.line[test__.all] = l;                    \
		test__.its[test__.all] = &test__body##id;       \
		test__.all++;                                   \
	}                                                       \
	void test__body##id(void)
/* Wrapper for IT__ macro because if __LINE__ is passed directly it
 * can't be concatenated with ## and used as ID.  But passing those
 * values through another macro function without doing anything
 * "unwraps" value making it possible to use it like text. */
#define IT__(msg, id) IT____(msg, id, __LINE__)
#define IT(msg) IT__(msg, __LINE__)

/* TODO(irek): Create pending test macro.  Call it TODO? */

#define ASSERT____(bool, onfail, line) do {                     \
		test__.fail = 0;                                \
		if (bool) break;                                \
		fprintf(stderr, "\n%s:%d: error: ",             \
			test__.fname, line);                    \
		onfail;                                         \
		putchar('\n');                                  \
		test__.fail = 1;                                \
		return;         /* Stop test on first fail */   \
	} while(0)
#define ASSERT__(bool, onfail) ASSERT____(bool, onfail, __LINE__)

#define ASSERT(x,msg) ASSERT__(x, fprintf(stderr, "%s", msg))
#define OK(x)         ASSERT(x, "'"#x"' not ok (it's 0)")
#define NUM_EQ(a,b)   ASSERT((a) == (b), "'"#a"' not equal to '"#b"'")
#define FAIL(msg)     ASSERT(0, msg)
#define STR_EQ(a,b)   ASSERT__(test_str_eq(a, b),                       \
			       fprintf(stderr,                          \
				       "not equal:\n\t'%s'\n\t'%s'",    \
				       a ? a : "<NULL>",                \
				       b ? b : "<NULL>"))
#define BUF_EQ(a,b,n) ASSERT__(test_buf_eq(a,b,n),                      \
			       fprintf(stderr,                          \
				       "not equal:\n\t'%-*s'\n\t'%-*s'",\
				       n, a, n, b))
#define PASS() do { test__.fail = 0; return; } while(0)

typedef struct {
	size_t    all;		       /* Number of all tests */
	char     *fname;	       /* Test file name */
	char     *msg[TESTMAX];	       /* Tests messages */
	size_t    line[TESTMAX];       /* Tests lines */
	void    (*its[TESTMAX])(void); /* Test functions pointers */
	int       fail;		       /* 1 when last test failed */
} TestState;

int  test_str_eq(char *a, char *b);
int  test_buf_eq(char *a, char *b, size_t n);
int  test__run(char *fname);

extern TestState test__;	/* Global warning  ^u^  */
       TestState test__ = {0, NULL, {NULL}, {0}, {NULL}, 0};

/* Check if given strings A and B are equal or both are NULL. */
int
test_str_eq(char *a, char *b)
{
	return (!a && !b) || strcmp(a, b) == 0;
}

/* Check if given buffers A and B of size N are equal. */
int
test_buf_eq(char *a, char *b, size_t n)
{
	return strncmp(a, b, n) == 0;
}

/* Runs all tests defined with IT macro.  Program returns number of
 * failed tests or 0 on success. */
int
main(void)
{
	size_t i, err = 0;	/* ERR to count failed tests */

	test__.fname = __FILE__;

	if (test__.all == 0) {
		fprintf(stderr, "No tests in '%s'\n", test__.fname);
		exit(1);
	}
	if (test__.all > TESTMAX) {
		fprintf(stderr, "More than %d tests (read doc)\n", TESTMAX);
		exit(1);
	}

	/* Run tests.  Print error on fail. */
	for (i = 0; i < test__.all; i++) {
		(*test__.its[i])();
		if (test__.fail) {
			err++;
			fprintf(stderr, "%s:%lu: error: failed test: '%s'\n",
				test__.fname, test__.line[i], test__.msg[i]);
			assert(err <= test__.all);
			test__.fail = 0;
		}
	}

	/* Print result */
	printf("TEST:\t%10s\t%.2lu tests\t", test__.fname, test__.all);

	if (err) printf("%.2lu FAILED\n", err);
	else     printf("ok\n");

	return err;
}

#endif	/* TEST_H_ */
