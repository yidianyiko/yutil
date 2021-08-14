#include "test.h"
#include "libtest.h"

int tests_count = 0;

int main()
{
	int ret = 0;

	describe("test list", test_list);
	describe("test math", test_math);
	describe("test test_logger", test_logger);
	describe("test test_list_entry", test_list_entry);
	describe("test test_dirent", test_dirent);
	describe("test test_dict", test_dict);

	return ret - print_test_result();
}