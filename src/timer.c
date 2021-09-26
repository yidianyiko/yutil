﻿/* timer.c -- timer support.
 *
 * Copyright (c) 2018, Liu chao <lc-soft@live.cn>
 * Copyright (c) 2021, Li Zihao <yidianyiko@foxmail.com>
 * All rights reserved.
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
#include <stdint.h>
#include "yutil/keywords.h"
#include "yutil/types.h"
#include "yutil/list.h"
#include "yutil/time.h"
#include "yutil/timer.h"

#define STATE_RUN 1
#define STATE_PAUSE 0

/*----------------------------- struct * --------------------------------*/
typedef struct timer_t_ timer_s_t;

struct timer_t_ {
	int state;    /**< 状态 */
	long int id;  /**< 定时器ID */
	bool_t reuse; /**< 是否重复使用该定时器 */

	int64_t start_time; /**< 定时器启动时的时间 */
	int64_t pause_time; /**< 定时器暂停时的时间 */
	long int total_ms;  /**< 定时时间（单位：毫秒） */
	long int pause_ms; /**< 定时器处于暂停状态的时长（单位：毫秒） */

	void (*callback)(void *); /**< 回调函数 */
	void *arg;                /**< 函数的参数 */

	list_node_t node; /**< 位于定时器列表中的节点 */
};
struct timer_list_t_ {
	int id_count;  /**< 定时器ID计数 */
	list_t timers; /**< 定时器数据记录 */
	bool_t active; /**< 定时器线程是否正在运行 */
};

/*----------------------------- Private ------------------------------*/

/** 更新定时器在定时器列表中的位置 */
static void timer_list_set_node(timer_list_t *list, list_node_t *node)
{
	timer_s_t *timer;
	int64_t t, tt;
	list_node_t *cur;
	/* 计算该定时器的剩余定时时长 */
	timer = (timer_s_t *)node->data;
	t = get_time_delta(timer->start_time);
	t = timer->total_ms - t + timer->pause_ms;
	list_for_each(cur, &list->timers)
	{
		timer = (timer_s_t *)cur->data;
		tt = get_time_delta(timer->start_time);
		tt = timer->total_ms - tt + timer->pause_ms;
		if (t <= tt) {
			list_link(&list->timers, cur->prev, node);
			return;
		}
	}
	list_append_node(&list->timers, node);
}

static timer_s_t *timer_find(timer_list_t *list, int timer_id)
{
	timer_s_t *timer;
	list_node_t *node;
	list_for_each(node, &list->timers)
	{
		timer = (timer_s_t *)node->data;
		if (timer && timer->id == timer_id) {
			return timer;
		}
	}
	return NULL;
}

static int timer_list_add(timer_list_t *list, long int n_ms,
			  timer_callback callback, void *arg, bool_t reuse)
{
	timer_s_t *timer;
	if (!list->active) {
		return -1;
	}

	timer = (timer_s_t *)malloc(sizeof(timer_s_t));
	if (timer == NULL)
		return -1;
	timer->arg = arg;
	timer->callback = callback;
	timer->reuse = reuse;
	timer->pause_ms = 0;
	timer->total_ms = n_ms;
	timer->state = STATE_RUN;
	timer->id = ++list->id_count;
	timer->start_time = get_time_ms();
	timer->node.next = NULL;
	timer->node.prev = NULL;
	timer->node.data = timer;
	timer_list_set_node(list, &timer->node);

	return timer->id;
}

/*----------------------------- Public ------------------------------*/

timer_list_t *timer_list_create()
{
	timer_list_t *list;
	list = (timer_list_t *)malloc(sizeof(timer_list_t));
	if (!list) {
		return NULL;
	}
	list->active = TRUE;
	list->id_count = 0;
	list_create(&list->timers);
	return list;
}

int timer_destroy(timer_list_t *list, int timer_id)
{
	timer_s_t *timer;
	if (!list->active) {
		return -2;
	}

	timer = timer_find(list, timer_id);
	if (!timer) {
		return -1;
	}
	list_unlink(&list->timers, &timer->node);
	free(timer);

	return 0;
}

int timer_pause(timer_list_t *list, int timer_id)
{
	timer_s_t *timer;
	if (!list->active) {
		return -2;
	}

	timer = timer_find(list, timer_id);
	if (timer) {
		/* 记录暂停时的时间 */
		timer->pause_time = get_time_ms();
		timer->state = STATE_PAUSE;
	}

	return timer ? 0 : -1;
}

int timer_continue(timer_list_t *list, int timer_id)
{
	timer_s_t *timer;
	if (!list->active) {
		return -2;
	}

	timer = timer_find(list, timer_id);
	if (timer) {
		/* 计算处于暂停状态的时长 */
		timer->pause_ms += (long int)get_time_delta(timer->pause_time);
		timer->state = STATE_RUN;
	}

	return timer ? 0 : -1;
}

int timer_reset(timer_list_t *list, int timer_id, long int n_ms)
{
	timer_s_t *timer;
	if (!list->active) {
		return -2;
	}

	timer = timer_find(list, timer_id);
	if (timer) {
		timer->pause_ms = 0;
		timer->total_ms = n_ms;
		timer->start_time = get_time_ms();
	}

	return timer ? 0 : -1;
}

int timer_list_add_timeout(timer_list_t *list, long int n_ms,
			   timer_callback callback, void *arg)
{
	return timer_list_add(list, n_ms, callback, arg, FALSE);
}

int timer_list_add_interval(timer_list_t *list, long int n_ms,
			    timer_callback callback, void *arg)
{
	return timer_list_add(list, n_ms, callback, arg, TRUE);
}

size_t timer_list_process(timer_list_t *list)
{
	size_t count = 0;
	long lost_ms;

	timer_s_t *timer = NULL;
	list_node_t *node;
	while (list && list->active) {
		list_for_each(node, &list->timers)
		{
			timer = (timer_s_t *)node->data;
			if (timer && timer->state == STATE_RUN) {
				break;
			}
		}
		if (!node) {
			break;
		}
		count += 1;
		lost_ms = (long)get_time_delta(timer->start_time);
		/* 若流失的时间未达到总定时时长 */
		if (lost_ms - timer->pause_ms < timer->total_ms) {
			break;
		}
		/* 若需要重复使用，则重置剩余等待时间 */
		list_unlink(&list->timers, node);
		timer->callback(timer->arg);
		if (timer->reuse) {
			timer->pause_ms = 0;
			timer->start_time = get_time_ms();
			timer_list_set_node(list, node);
		} else {
			free(timer);
		}
	}

	return count;
}

void timer_list_destroy(timer_list_t *list)
{
	if (!list->active) {
		return;
	}
	list->active = FALSE;
	list_destroy_without_node(&list->timers, free);
	free(list);
}
