/* dirent.c -- Directory entry operation set.
 *
 * Copyright (c) 2018, Liu chao <lc-soft@live.cn> All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of LCUI nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../yutil/dirent.h"

#ifdef _WIN32
#include <Windows.h>
typedef HANDLE dir_handle_t;

union dir_entry_t {
	WIN32_FIND_DATAA data_a;
	WIN32_FIND_DATAW data_w;
};
#else
#include <dirent.h>
#include <locale.h>
typedef DIR *dir_handle_t;

struct dir_entry_t {
	struct dirent dirent;
	wchar_t name[DIRENT_NAME_LEN];
};

#define DIRENT_NAME_LEN 256
#endif

struct dir_t_ {
	dir_handle_t handle;
	dir_entry_t entry;
	int cached;
};

int dir_open_a(const char *path, dir_t *dir)
{
#if defined(_WIN32)
	size_t len;
	char *newpath;
	char name[] = "\\*";

	len = strlen(path) + 1;
	newpath = malloc(len * sizeof(char) + sizeof(name));
	if (newpath == NULL) {
		return -ENOMEM;
	}
	strcpy(newpath, path);
	strcpy(newpath + len - 1, name);
	dir->handle = FindFirstFileA(newpath, &dir->entry.data_a);
	free(newpath);
	if (dir->handle == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_FILE_NOT_FOUND:
			return -ENOENT;
		case ERROR_ACCESS_DENIED:
			return -EACCES;
		default:
			break;
		}
		return -1;
	}
	dir->cached = 1;
#endif
	return 0;
}

int dir_open_w(const wchar_t *path, dir_t *dir)
{
#if defined(_WIN32)
	size_t len;
	wchar_t *newpath;
	wchar_t name[] = L"\\*";

	len = wcslen(path) + 1;
	newpath = malloc(len * sizeof(wchar_t) + sizeof(name));
	if (!newpath) {
		return -ENOMEM;
	}
	wcscpy(newpath, path);
	wcscpy(newpath + len - 1, name);
	dir->handle = FindFirstFileW(newpath, &dir->entry.data_w);
	free(newpath);
	if (dir->handle == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_FILE_NOT_FOUND:
			return -ENOENT;
		case ERROR_ACCESS_DENIED:
			return -EACCES;
		default:
			break;
		}
		return -1;
	}
	dir->cached = 1;
#else

	int len;
	char *newpath;
	if (!(setlocale(LC_ALL, "C.UTF-8") ||
	      setlocale(LC_ALL, "en_US.UTF-8") ||
	      setlocale(LC_ALL, "zh_CN.UTF-8"))) {
		setlocale(LC_ALL, "");    //"failed to setlocale to utf-8"
	}
	len = wcstombs(NULL, path, 0) + 1;
	newpath = malloc(len * sizeof(wchar_t));
	wcstombs(newpath, path, len);
	setlocale(LC_ALL, "C");
	dir->handle = opendir(newpath);
	free(newpath);
	if (!dir->handle) {
		return -1;
	}
#endif
	return 0;
}

dir_entry_t *dir_read_a(dir_t *dir)
{
#if defined(_WIN32)
	if (dir->handle == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	if (dir->cached) {
		dir->cached = 0;
		return &dir->entry;
	}
	if (FindNextFileA(dir->handle, &dir->entry.data_a)) {
		return &dir->entry;
	}
	FindClose(dir->handle);
	dir->handle = INVALID_HANDLE_VALUE;
#endif
	return NULL;
}

dir_entry_t *dir_read_w(dir_t *dir)
{
#if defined(_WIN32)
	if (dir->handle == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	if (dir->cached) {
		dir->cached = 0;
		return &dir->entry;
	}
	if (FindNextFileW(dir->handle, &dir->entry.data_w)) {
		return &dir->entry;
	}
	FindClose(dir->handle);
	dir->handle = INVALID_HANDLE_VALUE;
	return NULL;
#else
	int len;
	struct dirent *d;
	d = readdir(dir->handle);
	if (!d) {
		return NULL;
	}
	dir->entry.dirent = *d;
	if (!(setlocale(LC_ALL, "C.UTF-8") ||
	      setlocale(LC_ALL, "en_US.UTF-8") ||
	      setlocale(LC_ALL, "zh_CN.UTF-8"))) {
		setlocale(LC_ALL, "");    //"failed to setlocale to utf-8"
	}
	len = mbstowcs((dir->entry.name, d->d_name,
				DIRENT_NAME_LEN);
	setlocale(LC_ALL, "C");
	dir->entry.name[len] = 0;
	return &dir->entry;
#endif
}

int dir_close(dir_t *dir)
{
#if defined(_WIN32)
	if (!FindClose(dir->handle)) {
		return -1;
	}
#else
	closedir(dir->handle);
#endif
	return 0;
}

char *dir_get_file_name_a(dir_entry_t *entry)
{
#if defined(_WIN32)
	return entry->data_a.cFileName;
#endif
	return NULL;
}

wchar_t *dir_get_file_name_w(dir_entry_t *entry)
{
#if defined(_WIN32)
	return entry->data_w.cFileName;
#else
	return entry->name;
#endif
	return NULL;
}

int file_is_directory(dir_entry_t *entry)
{
#if defined(_WIN32)
	return entry->data_w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#else
	return entry->dirent.d_type == DT_DIR;
#endif
}

int file_is_regular(dir_entry_t *entry)
{
#if defined(_WIN32)
	return !(entry->data_w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
	return entry->dirent.d_type == DT_REG;
#endif
}
