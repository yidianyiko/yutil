#include <wchar.h>
#include <string.h>
#include "../include/keywords.h"
#include "../include/yutil/charset.h"

#include "test.h"

void test_charset(void)
{
	size_t len;
	char str[64];
	wchar_t wcs[64];

	len = decode_utf8(wcs, "hello", 64);
	it_b("test decode ascii string", len == 5 && wcscmp(wcs, L"hello") == 0,
	     TRUE);
#ifdef _WIN32
	len = decode_string(wcs, "简体中文", 64, ENCODING_ANSI);
	it_b("test decode ansi string",
	     len == 4 && wcscmp(wcs, L"简体中文") == 0, TRUE);

	len = encode_string(str, L"简体中文", 64, ENCODING_ANSI);
	it_b("test encode unicode string to ansi",
	     len == strlen("简体中文") && strcmp(str, "简体中文") == 0, TRUE);
#else
	len = decode_utf8(wcs, "简体中文", 64);
	it_b("test decode utf-8 string",
	     len == 4 && wcscmp(wcs, L"简体中文") == 0, TRUE);

	len = encode_utf8(str, L"简体中文", 64);
	it_b("test encode unicode string to utf-8",
	     len == strlen("简体中文") && strcmp(str, "简体中文") == 0, TRUE);
#endif
	len = encode_utf8(str, L"hello", 64);
	it_b("test encode ascii string to utf-8",
	     len == 5 && strcmp(str, "hello") == 0, TRUE);
}
