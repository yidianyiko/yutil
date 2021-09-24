## 字符编码 charset

支持 UTF-8、ANSI 与 Unicode 间的字符编码转换，且中英文混用时不会有乱码。
下面举个例子：
```c
void charset(void)
{
	size_t len;
	char str[64];
	wchar_t wcs[64];

  //返回实际转换字符的长度
	len = decode_string(wcs, "简体中文", 64, ENCODING_ANSI);

	len = encode_string(str, L"简体中文
}
```