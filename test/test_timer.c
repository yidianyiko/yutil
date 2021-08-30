#include <windows.h>
#include <process.h> /* _beginthread, _endthread */
#include <stdint.h>
#include <stdio.h>
#include "test.h"
#include "libtest.h"
#include "../include/keywords.h"
#include "../include/yutil/timer.h"
#include "../include/yutil/time.h"

static timer_list_t *timer_list = NULL;
static int count = 0;
static HANDLE mutex;

void on_time_out(void *arg)
{
	int *timer_id = arg;

	it_b("check timer_free()", timer_destroy(*timer_id, timer_list) == 0,
	     TRUE);
	timer_list_destroy(timer_list);
	count = -1;
	_endthread();
}

void on_interval(void *arg)
{
	count++;
}

void process(void *ignored)
{
	while (1) {
		WaitForSingleObject(mutex, INFINITE);
		timer_list_process(timer_list);
		ReleaseMutex(mutex);
		msleep(20L);
	}
}

void test_timer(void)
{
	int timer_id;
	int ret = count;
	mutex = CreateMutex(NULL, FALSE, NULL);

	it_b("check timer_list_new()",
	     (timer_list = timer_list_create()) != NULL, TRUE);
	it_b("check timer_list_add_interval()",
	     (timer_id =
		  timer_list_add_interval(10, on_interval, 0, timer_list)) == 1,
	     TRUE);

	it_b("check timer_list_add_timeout()",
	     timer_list_add_timeout(250, on_time_out, &timer_id, timer_list) ==
		 2,
	     TRUE);

	// process
	_beginthread(process, 0, NULL);
	msleep(50L);
	ret = count;
	// test pause
	WaitForSingleObject(mutex, INFINITE);
	timer_pause(timer_id, timer_list);
	ReleaseMutex(mutex);
	msleep(50L);
	it_b("check timer_pause()", (ret != 0) && (ret == count), TRUE);

	// test continue
	WaitForSingleObject(mutex, INFINITE);
	timer_continue(timer_id, timer_list);
	ReleaseMutex(mutex);
	msleep(50L);
	it_b("check timer_continue()", ret < count, TRUE);

	count = 0;
	// test reset
	WaitForSingleObject(mutex, INFINITE);
	timer_reset(timer_id, 30, timer_list);
	ReleaseMutex(mutex);
	msleep(50L);
	it_i("check timer_reset()", count, 1);

	msleep(50L);
	it_b("check timer_list_process()", count == -1, TRUE);
}