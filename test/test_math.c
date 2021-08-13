#include "test.h"
#include "libtest.h"
#include "../include/yutil/math.h"

void test_math_max(void)
{
	it_b("max() should work", (int)max(1, 2) == 2, TRUE);
}

void test_math_min(void)
{
	it_b("min() should work", (int)min(1, 2) == 1, TRUE);
}

void test_math_round(void)
{
	it_b("round() should work", (int)round(2.3) == 2, TRUE);
}

void test_math(void)
{
	test_math_max();
	test_math_min();
	test_math_round();
}