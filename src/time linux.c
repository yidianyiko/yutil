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
#include <stdint.h>
#ifndef _WIN32
#include <time.h>
#include "../include/keywords.h"
#include "../include/yutil/time.h"

#define TIME_WRAP_VALUE (~(int64_t)0)

#include <unistd.h>
#include <sys/time.h>

struct _timeval_t {
	int64_t tv_sec;
	int64_t tv_usec;
};

void time_init(void)
{
	return;
}

int64_t get_time(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return ((int64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}

int64_t get_time_delta(int64_t start)
{
	int64_t now = get_time();
	if (now < start) {
		return (TIME_WRAP_VALUE - start) + now;
	}
	return now - start;
}

void msleep(unsigned int ms)
{
	usleep(ms * 1000);
}

void sleep(unsigned int s)
{
	msleep(s * 1000);
}

// get the time from 1970-01-01 00:00:00:000
void get_time_of_day(timeval_t *tv)
{
	struct timeval tmp = { 0 };
	if (gettimeofday(&tmp, NULL))
		return;

	t->.tv_sec = (int64_t)tmp.tv_sec;
	tv->tv_usec = (int64_t)tmp.tv_usec;
	return;
}

int64_t get_utime()
{
	timeval_t tv = { 0 };
	get_time_of_day(&tv);
	return ((int64_t)tv.tv_sec * 1000000 + tv.tv_usec);
}
#endif