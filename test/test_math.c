#include "test.h"
#include "libtest.h"
#include "../include/yutil/keywords.h"
#include "../include/yutil/math.h"

static void test_math_max(void)
{
	it_b("max() should work", (int)max(1, 2) == 2, TRUE);
}

static void test_math_min(void)
{
	it_b("min() should work", (int)min(1, 2) == 1, TRUE);
}

static void test_math_round(void)
{
	it_b("round() should work", (int)round(2.3) == 2, TRUE);
}

static void test_math_pow2(void)
{
	it_b("pow2() should work", (int)pow2(2) == 4, TRUE);
}

void test_math(void)
{
	test_math_max();
	test_math_min();
	test_math_round();
	test_math_pow2();
}