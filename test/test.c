#include "test.h"
#include "libtest.h"

int tests_count = 0;

int main()
{
	int ret = 0;

	describe("test list", test_list);
	describe("test math", test_math);
	return ret - print_test_result();
}