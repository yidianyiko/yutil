/* time.c -- The time operation set.
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

#include <time.h>
#include <stdint.h>

#include "../include/yutil/time.h"

#define TIME_WRAP_VALUE (~(int64_t)0)

#ifdef _WIN32
#include <Windows.h>

static int HIRES_TIMER_AVAILABLE = 0; /**< 标志，指示高精度计数器是否可用 */
static LONGLONG HIRES_TICKS_PER_SECOND; /**< 高精度计数器每秒的滴答数 */
#else
#include <unistd.h>
#include <sys/time.h>
#endif

void time_init(void)
{
#ifdef _WIN32
	LARGE_INTEGER hires;
	if (QueryPerformanceFrequency(&hires)) {
		HIRES_TIMER_AVAILABLE = 1;
		HIRES_TICKS_PER_SECOND = hires.QuadPart;
	}
#else
	return;
#endif
}

int64_t time_get(void)
{
#ifdef _WIN32
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
#else
	int64_t t;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	t = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return t;
#endif
}

int64_t time_get_delta(int64_t start)
{
	int64_t now = time_get();
	if (now < start) {
		return (TIME_WRAP_VALUE - start) + now;
	}
	return now - start;
}

void sleep(unsigned int s)
{
#ifdef _WIN32
	Sleep(s * 1000);
#else
	sleep(s);
#endif
}

void msleep(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}
