#include "test.h"
#include "libtest.h"
#include "yutil/keywords.h"
#include "yutil/math.h"

static void test_math_max(void)
{
	it_b("y_max() should work", (int)y_max(1, 2) == 2, TRUE);
}

static void test_math_min(void)
{
	it_b("y_min() should work", (int)y_min(1, 2) == 1, TRUE);
}

static void test_math_round(void)
{
	it_b("y_round() should work", (int)y_round(2.3) == 2, TRUE);
}

static void test_math_pow2(void)
{
	it_b("y_pow2() should work", (int)y_pow2(2) == 4, TRUE);
}

void test_math(void)
{
	test_math_max();
	test_math_min();
	test_math_round();
	test_math_pow2();
}