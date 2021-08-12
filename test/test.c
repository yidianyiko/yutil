#include "test.h"
#include "libtest.h"

int tests_count = 0;

int main()
{
	int ret = 0;

	describe("test linkedlist", test_list);
	return ret - print_test_result();
}