#include "test.h"
#include "libtest.h"

int main()
{
	int ret = 0;

	describe("test list", test_list);
	describe("test math", test_math);
	describe("test test_logger", test_logger);
	describe("test test_list_entry", test_list_entry);
	describe("test test_dirent", test_dirent);
	describe("test test_string", test_strs);
	describe("test test_strpool", test_strpool);
	describe("test test_timer", test_timer);
	describe("test test_charset", test_charset);
	describe("test test_dict", test_dict);
	describe("test test_rbtree", test_rbtree);
	return ret - print_test_result();
}