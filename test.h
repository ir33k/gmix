/* Single header library for creating unit test programs.
 *
 * Can be used (included) only in one test program because it has on
 * single global tests state, it defines it's own "main" function and
 * TEST macro relays on file line numbers.  You can define only 64
 * tests by default but this can be changed by predefining TESTMAX
 * (see example).  Variables, functions and macros not mentioned in
 * example test program should not be used.
 *
 * Example test program:
 *
 *	// File: program.t.c
 *	#define TESTMAX 32              // Define to handle more tests
 *	#include "test.h"               // Get test lib
 *
 *	TEST("Should pass")             // Define test with test cases
 *	{
 *		// Defined test will end on first failing test case
 *		OK(bool);               // Is boolean true?
 *		EQ(num1, num2);         // Are numbers equal?
 *		STR_EQ(s1, s2);         // Are strings equal?
 *		BUF_EQ(b1, b2, size);   // Are buffers equal?
 *
 *		// ASSERT is like OK but with custom fail message
 *		ASSERT(bool, "fail message");
 *
 *		// Helper functions that returns boolean
 *		OK(test_str_eq(str1, str2));
 *		OK(test_buf_eq(str1, str2, size));
 *
 *		FAIL("fail message");   // End test here with fail
 *		PASS();                 // End test here with success
 *	}
 *
 *	TEST("Another test 1") { ... }  // Define as many as TESTMAX
 *	TEST("Another test 2") { ... }
 *
 *	// Note that there is no "main" function
 *
 * Compile and run "program" tests with:
 *
 *	$ cc -o program.t program.t.c && ./program.t
 */
#ifdef TEST_H_
#error "test.h library can't be used in multiple files (read doc)"
#else
#define TEST_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef TESTMAX			/* Predefined for more tests */
#define TESTMAX    64		/* Maximum number of tests */
#endif

#define TEST____(_msg, id, _line)                               \
	void test__body##id(void);                              \
	/* This function will be run before "main" function.  It's the
	 * heart, main hack, my own precious ring of power that makes
	 * this test lib possible. */                           \
	void test__##id(void) __attribute__ ((constructor));    \
	void test__##id(void) {                                 \
		if (test__.all == 0) {         /* Initialize */ \
			test__.fname = __FILE__;                \
		}                                               \
		test__.msg[test__.all] = _msg;                  \
		test__.line[test__.all] = _line;                \
		test__.its[test__.all] = &test__body##id;       \
		test__.all++;                                   \
	}                                                       \
	void test__body##id(void)
/* Intermidiate macro function TEST__ is necessary to "unwrap"
 * __LINE__ macro so it could be used as a ID string. */
#define TEST__(msg, id) TEST____(msg, id, __LINE__)
#define TEST(msg) TEST__(msg, __LINE__)

/* TODO(irek): Create pending test macro.  Call it TODO? */

#define ASSERT____(bool, onfail, line) do {                     \
		if (bool) break;            /* Pass */          \
		test__.fail = 1;            /* Fail */          \
		fprintf(stderr, "\n%s:%d: error: ",             \
			test__.fname, line);                    \
		onfail;                                         \
		fputc('\n', stderr);                            \
		return;         /* Stop test on first fail */   \
	} while(0)
#define ASSERT__(bool, onfail) ASSERT____(bool, onfail, __LINE__)

#define ASSERT(x,msg) ASSERT__(x, fputs(msg, stderr))
#define OK(x)         ASSERT(x, "'"#x"' not ok (it's 0)")
#define EQ(a,b)       ASSERT((a) == (b), "'"#a"' not equal to '"#b"'")
#define FAIL(msg)     ASSERT(0, msg)
#define STR_EQ(a,b)   ASSERT__(test_str_eq(a, b),                       \
			       fprintf(stderr,                          \
				       "not equal:\n\t'%s'\n\t'%s'",    \
				       a ? a : "<NULL>",                \
				       b ? b : "<NULL>"))
#define BUF_EQ(a,b,n) ASSERT__(test_buf_eq(a,b,n),                      \
			       fprintf(stderr,                          \
				       "not equal:\n\t'%-*s'\n\t'%-*s'", \
				       n, a, n, b))
#define PASS() do { test__.fail = 0; return; } while(0)

typedef struct {
	size_t    all;                 /* Number of all tests */
	char     *fname;               /* Test file name */
	char     *msg[TESTMAX];        /* Tests messages */
	size_t    line[TESTMAX];       /* Tests lines */
	void    (*its[TESTMAX])(void); /* Test functions pointers */
	int       fail;	               /* 1 when last test failed */
} TestState;

int  test_str_eq(char *a, char *b);
int  test_buf_eq(char *a, char *b, size_t n);
int  test__run(char *fname);

extern TestState test__;         /* Global warning  ^u^  */
       TestState test__ = {0, NULL, {NULL}, {0}, {NULL}, 0};

/* Check if given strings A and B are equal or both are NULL. */
int
test_str_eq(char *a, char *b)
{
	return (!a && !b) || (strcmp(a, b) == 0);
}

/* Check if given buffers A and B of size N are equal. */
int
test_buf_eq(char *a, char *b, size_t n)
{
	return strncmp(a, b, n) == 0;
}

/* Runs all tests defined with TEST macro.  Program returns number of
 * failed tests or 0 on success. */
int
main(void)
{
	size_t   i, err = 0;	/* ERR to count failed tests */

	if (test__.all > TESTMAX) {
		fprintf(stderr, "ERROR: More than %d tests (read doc)\n",
			TESTMAX);
		return 1;
	}

	/* Run tests.  Print error on fail. */
	for (i = 0; i < test__.all; i++) {
		test__.fail = 0;
		(*test__.its[i])();    /* Run */

		if (test__.fail == 0)
			continue;      /* Pass */

		err++;                 /* Fail */
		fprintf(stderr, "%s:%lu: error: failed test: '%s'\n",
			test__.fname, test__.line[i], test__.msg[i]);
		assert(err <= test__.all);
	}

	/* Print result */
	printf("TEST:\t%10s\t%.2lu tests\t", test__.fname, test__.all);

	if (err) printf("%.2lu FAILED\n", err);
	else     puts("ok\n");

	return err;
}

#endif	/* TEST_H_ */
