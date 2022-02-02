﻿#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "libtest.h"
#include "yutil/keywords.h"
#include "yutil/time.h"

#ifdef _WIN32
#define COLOR_NONE
#define COLOR_RED
#define COLOR_GREEN
#else
#define COLOR_NONE "\e[0m"
#define COLOR_RED "\e[0;31m"
#define COLOR_GREEN "\e[0;32m"
#endif
#define RED(TEXT) COLOR_RED TEXT COLOR_NONE
#define GREEN(TEXT) COLOR_GREEN TEXT COLOR_NONE

#if defined(CI_ENV) || defined(_WIN32)
#define CHECK_MARK GREEN("* ")
#define ERROR_MARK "x "
#else
#define CHECK_MARK GREEN("√ ")
#define ERROR_MARK "× "
#endif

static size_t tests_passed = 0;
static size_t tests_total = 0;
static int test_msg_indent = 0;
static int64_t test_start_time = 0;

static int test_msg(const char *fmt, ...)
{
	int ret;
	char str[1024];
	va_list args;

	va_start(args, fmt);
	ret = vsnprintf(str, 1023, fmt, args);
	va_end(args);
	printf("%*s%s", test_msg_indent * 2, "", str);
	return ret;
}

void test_begin(void)
{
	if (test_start_time == 0) {
		test_start_time = get_time_ms();
	}
	test_msg_indent++;
}

void test_end(void)
{
	test_msg_indent--;
}

int test_result(void)
{
	if (tests_total > tests_passed) {
		return (int)(tests_total - tests_passed);
	}
	return 0;
}

int print_test_result(void)
{
	printf(GREEN("  %zu passing") " (%ums)\n", tests_passed,
	       (unsigned)get_time_delta(test_start_time));
	if (tests_total > tests_passed) {
		printf(RED("  %zu faling\n\n"), tests_total - tests_passed);
		return (int)(tests_total - tests_passed);
	}
	printf("\n");
	return 0;
}

void describe(const char *name, void (*func)())
{
	test_msg("%s\n", name);
	test_begin();
	func();
	test_end();
	test_msg("\n");
}

void it_i(const char *name, int actual, int expected)
{
	tests_total++;
	if (actual == expected) {
		test_msg(CHECK_MARK "%s == %d\n", name, expected);
		tests_passed++;
		return;
	}
	test_msg(RED(ERROR_MARK "%s == %d\n"), name, expected);
	test_msg(RED("  AssertionError: %d == %d\n"), actual, expected);
	test_msg(GREEN("  + expected ") RED("- actual\n\n"));
	test_msg(RED("  - %d\n"), actual);
	test_msg(GREEN("  + %d\n\n"), expected);
}

void it_b(const char *name, int actual, int expected)
{
	const char *actual_str = actual ? "true" : "false";
	const char *expected_str = expected ? "true" : "false";

	tests_total++;
	if (!actual == !expected) {
		test_msg(CHECK_MARK "%s\n", name);
		tests_passed++;
		return;
	}
	test_msg(RED(ERROR_MARK "%s\n"), name);
	test_msg(RED("  AssertionError: %s == %s\n"), actual_str, expected_str);
	test_msg(GREEN("  + expected ") RED("- actual\n\n"));
	test_msg(RED("  - %s\n"), actual_str);
	test_msg(GREEN("  + %s\n\n"), expected_str);
}

void it_s(const char *name, const char *actual, const char *expected)
{
	tests_total++;
	if ((actual && expected && strcmp(actual, expected) == 0) ||
	    actual == expected) {
		test_msg(CHECK_MARK "%s == '%s'\n", name, expected);
		tests_passed++;
		return;
	}
	test_msg(RED(ERROR_MARK "%s == '%s'\n"), name, expected);
	if (expected) {
		test_msg(RED("  AssertionError: '%s' == '%s'\n"), actual,
			 expected);
	} else {
		test_msg(RED("  AssertionError: '%s' == null\n"), actual);
	}
	test_msg(GREEN("  + expected ") RED("- actual\n\n"));
	test_msg(RED("  - %s\n"), actual);
	test_msg(GREEN("  + %s\n\n"), expected);
}
