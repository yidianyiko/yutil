/*
 * dirent.h -- Directory entry operation set.
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

#ifndef UTIL_DIRENT_H
#define UTIL_DIRENT_H

#ifdef _WIN32
#include <Windows.h>

#define PATH_DELIMITER '\\'

typedef HANDLE dir_handle_t;
typedef union dir_entry_t dir_entry_t;
#else
#include <dirent.h>

#define DIRENT_NAME_LEN 256
#define PATH_DELIMITER '/'

typedef DIR *dir_handle_t;
typedef struct dir_entry_t dir_entry_t;
#endif

typedef struct dir_t_ dir_t;

#if defined(_UNICODE) || !defined(_WIN32)
#define dir_open dir_open_w
#define dir_read dir_read_w
#define dir_get_file_name dir_get_file_name_w
#else
#define dir_open dir_open_a
#define dir_read dir_read_a
#define dir_get_file_name dir_get_file_name_a
#endif

int dir_open_w(const wchar_t *path, dir_t *dir);

int dir_open_a(const char *path, dir_t *dir);

dir_entry_t *dir_read_a(dir_t *dir);

dir_entry_t *dir_read_w(dir_t *dir);

int dir_close(dir_t *dir);

char *dir_get_file_name_a(dir_entry_t *entry);

wchar_t *dir_get_file_name_w(dir_entry_t *entry);

int file_is_directory(dir_entry_t *entry);

int file_is_regular(dir_entry_t *entry);

#endif
