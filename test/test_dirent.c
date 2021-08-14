#include "test.h"
#include "libtest.h"
#include <locale.h>

#include "../include/yutil/dirent.h"

#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define PATH_LEN 2048

static dir_t* dir;

static void test_dirent_a()
{
	dir_entry_t* entry;
	size_t len;
	size_t dir_len = 0;
	char* name;
	char path[PATH_LEN] = { 0 };

#if defined(_WIN32)
	wchar_t path_w[PATH_LEN] = { 0 };
	GetCurrentDirectoryW(PATH_LEN, path_w);

	setlocale(LC_ALL, "");
	wcstombs(path, path_w, PATH_LEN);
	setlocale(LC_ALL, "C");
	printf("%s", path);
#else
	if (!getcwd(path, PATH_LEN - 1)) {
		return;
	}
#endif
	len = strlen(path);

	it_b("dir_open_a() should work", dir_open_a(path, &dir) == 0, TRUE);

	while ((entry = dir_read_a(&dir))) {
		dir_len++;
		name = dir_get_file_name_a(entry);
		if (name[0] == '.') {
			continue;
		} else {
			it_b("dir_get_file_name_a() should work", name, TRUE);
			break;
		}
	}
	it_b("dir_read_a() should work", dir_len != 0, TRUE);
}
static void test_dirent_w(void)
{
	dir_entry_t* entry;
	size_t len;
	size_t dir_len = 0;
	wchar_t* name_w;

	wchar_t path_w[PATH_LEN] = { 0 };

#if defined(_WIN32)
	GetCurrentDirectoryW(PATH_LEN, path_w);
#else
	char path[PATH_LEN] = { 0 };
	if (getcwd(path, PATH_LEN - 1))
		if (!(setlocale(LC_ALL, "C.UTF-8") ||
		      setlocale(LC_ALL, "en_US.UTF-8") ||
		      setlocale(LC_ALL, "zh_CN.UTF-8"))) {
			setlocale(LC_ALL, "");
		}
	mbstowcs(path_w, path, PATH_LEN);
	setlocale(LC_ALL, "C");
#endif
	len = wcslen(path_w);
	it_b("dir_open_w() should work", dir_open_w(path_w, &dir) == 0, TRUE);

	while ((entry = dir_read_w(&dir))) {
		dir_len++;
		name_w = dir_get_file_name_w(entry);
		if (name_w[0] == '.') {
			continue;
		} else {
			it_b("dir_get_file_name_w() should work", name_w, TRUE);

			if (dir_file_is_directory(entry)) {
				it_b("dir_file_is_directory() should work",
				     dir_file_is_directory(entry), TRUE);
				it_b("dir_file_is_regular() should work",
				     !dir_file_is_regular(entry), TRUE);
				break;
			} else {
				it_b("dir_file_is_directory() should work",
				     !dir_file_is_directory(entry), TRUE);
				it_b("dir_file_is_regular() should work",
				     dir_file_is_regular(entry), TRUE);
				break;
			}
		}
	}
	it_b("dir_read_w() should work", dir_len != 0, TRUE);
}

void test_dirent(void)
{
	test_dirent_a();
	test_dirent_w();
}