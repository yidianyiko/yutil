#include <stdio.h>
#include "test.h"
#include "libtest.h"
#include "../include/keywords.h"
#include "../include/yutil/timer.h"
static timer_list_t *timer_list;

static int count = 1; 

void on_time_out(void *arg)
{
	
	int *timer_id = arg;

	it_b("check timer_pause()", timer_pause(*timer_id, timer_list) == 0,
	     TRUE);
	it_b("check timer_continue()",
	     timer_continue(*timer_id, timer_list) == 0,
	     TRUE);
	it_b("check timer_reset()", timer_reset(*timer_id,100 ,timer_list) == 0,
	     TRUE);
	it_b("check timer_free()", timer_free(*timer_id, timer_list) == 0,
	     TRUE);
	timer_list_remove(timer_list);
	it_b("check timer_list_remove()",1,
	     TRUE);
	count = 0;
}

void on_interval(void *arg)
{
	count++;
	if (count == 5)
		it_i("check timer_list_add_interval()", count, 5);
}

void test_timer(void)
{
	int timer_id;
	
	
	it_b("check timer_list_new()", (timer_list = timer_list_new()) != NULL,
	     TRUE);
	it_b("check timer_list_add_interval()",
	     (timer_id = timer_list_add_interval(100, on_interval, 0,
						 timer_list)) == 1, TRUE);

		it_b("check timer_list_add_timeout()",
	     timer_list_add_timeout(500, on_time_out, &timer_id, timer_list) ==
		 2,
	     TRUE);

	while (count) {
		timer_list_process(timer_list);
	}
	it_b("check timer_list_process()",
	     count == 0,
	     TRUE);
	timer_list_destory(timer_list);
}