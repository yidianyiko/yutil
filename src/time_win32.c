﻿/* time.c -- The time operation set.
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
#if defined(_WIN32)
#include <stdint.h>
#include <Windows.h>
#include "../include/keywords.h"
#include "../include/yutil/time.h"

#define TIME_WRAP_VALUE (~(int64_t)0)

static int HIRES_TIMER_AVAILABLE = 0; /**< 标志，指示高精度计数器是否可用 */
static LONGLONG HIRES_TICKS_PER_SECOND; /**< 高精度计数器每秒的滴答数 */

struct _timeval_t {
	int64_t tv_sec;
	int64_t tv_usec;
};

void time_init(void)
{
	LARGE_INTEGER hires;
	if (QueryPerformanceFrequency(&hires)) {
		HIRES_TIMER_AVAILABLE = 1;
		HIRES_TICKS_PER_SECOND = hires.QuadPart;
	}
}

int64_t get_time(void)
{
	int64_t time;
	LARGE_INTEGER hires_now;
	FILETIME *ft = (FILETIME *)&time;
	if (HIRES_TIMER_AVAILABLE) {
		QueryPerformanceCounter(&hires_now);
		time = hires_now.QuadPart * 1000;
		return time / HIRES_TICKS_PER_SECOND;
	}
	GetSystemTimeAsFileTime(ft);
	return time / 1000 - 11644473600000;
}

int64_t get_time_delta(int64_t start)
{
	int64_t now = get_time();
	return (now < start) ? ((TIME_WRAP_VALUE - start) + now)
			     : (now - start);
}

void msleep(unsigned int ms)
{
	Sleep((DWORD)ms);
}

void sleep(unsigned int s)
{
	msleep(s * 1000);
}

// get the time from 1970-01-01 00:00:00:000
void get_time_of_day(timeval_t *tv)
{
	union {
		uint64_t ns100;
		FILETIME ft;

	} now;

	if (tv) {
		GetSystemTimeAsFileTime(&now.ft);
		tv->tv_sec = (int64_t)((now.ns100 - 116444736000000000ULL) /
				       10000000ULL);
		tv->tv_usec = (int64_t)((now.ns100 / 10ULL) % 1000000ULL);
	}
	return;
}

int64_t get_utime()
{
	if (HIRES_TIMER_AVAILABLE) {
		LARGE_INTEGER hires_now = { { 0 } };
		if (!QueryPerformanceFrequency(&hires_now))
			return 0;

		return (hires_now.QuadPart * 1000000) / HIRES_TICKS_PER_SECOND;
	}

	timeval_t tv = { 0 };
	get_time_of_day(&tv);
	return ((int64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}
#endif