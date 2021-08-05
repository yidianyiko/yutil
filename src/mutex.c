/* thread.c -- The pthread edition thread opreation set.
 *
 * Copyright (c) 2018-2019, Liu chao <lc-soft@live.cn> All rights reserved.
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
#include "../include/mutex.h"

#ifdef _WIN32

int mutex_init(mutex_t *mutex)
{
	*mutex = CreateMutex(NULL, FALSE, NULL);
	return 0;
}

/* Free the mutex */
void mutex_destroy(mutex_t *mutex)
{
	CloseHandle(*mutex);
}

/* Try lock the mutex */
int mutex_try_lock(mutex_t *mutex)
{
	switch (WaitForSingleObject(*mutex, 0)) {
	case WAIT_FAILED:
		return -1;
	case WAIT_OBJECT_0:
		break;
	default:
		return -2;
	}
	return 0;
}

/* Lock the mutex */
int mutex_lock(mutex_t *mutex)
{
	if (WaitForSingleObject(*mutex, INFINITE) == WAIT_FAILED) {
		return -1;
	}
	return 0;
}

/* Unlock the mutex */
int mutex_unlock(mutex_t *mutex)
{
	if (ReleaseMutex(*mutex) == FALSE) {
		return -1;
	}
	return 0;
}
#else

/* init the mutex */
int mutex_Init(mutex_t *mutex)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	return pthread_mutex_init(mutex, &attr);
}

/* Free the mutex */
void mutex_destroy(mutex_t *mutex)
{
	pthread_mutex_destroy(mutex);
}

/* Try lock the mutex */
int mutex_try_lock(mutex_t *mutex)
{
	return pthread_mutex_tryLock(mutex);
}

/* Lock the mutex */
int mutex_lock(mutex_t *mutex)
{
	return pthread_mutex_lock(mutex);
}

/* Unlock the mutex */
int mutex_unlock(mutex_t *mutex)
{
	return pthread_mutex_unlock(mutex);
}
#endif
