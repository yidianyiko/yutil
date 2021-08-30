#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "libtest.h"
#include "../include/keywords.h"
#include "../include/yutil/rbtree.h"
#include "../include/yutil/time.h"

static const int count = 1 << 20;
static const int max_len = 15;

void test_rbtree(void)
{
	int i;
	int c = 0;
	const int delete_count = 1024;
	const int search_count = 1 << 10;
	rbtree_t tree = { 0 };
	rbtree_node_t *s, *node;
	rbtree_init(&tree);

	char* v[] = {
		"apple", "banana", "cherry",    "grape",      "lemon",
		"mango", "pear",   "pineapple", "strawberry", "watermelon"
	};

	long long time1 = (long long)get_time();
	for (i = 0; i < count; i++) {
		rbtree_insert(&tree, i + 1, NULL, v[i % 10]);
	}
	printf("Inserting %d nodes spends %lld ms.\n", count,
	       (long long)time_get_delta(time1));

	for (node = rbtree_get_min(tree.root); node; node = rbtree_next(node)) {
		c++;
	}
	it_i("rbtree_insert() should work", c, count);

	c = 0;

	srand((unsigned int)time(0));
	for (i = 0; i < search_count; i++) {
		s = rbtree_search_by_key(&tree, (rand() % count) + 1);
		if (s) {
			c++;
		}
	}

	long long time2 = (long long)get_time();
	printf("Searching %d nodes among %d spends %lld seconds.\n",
	       search_count, count, time_get_delta(time2));
	it_i("rbtree_search_by_key() should work", c, search_count);

	for (i = 1; i <= delete_count; i++) {
		rbtree_delete(&tree, i, NULL);
	}

	long long time3 = (long long)get_time();
	printf("Deleting %d nodes among %d spends %lld seconds.\n",
	       delete_count, count, time_get_delta(time3));
	c = 0;
	for (node = rbtree_get_min(tree.root); node; node = rbtree_next(node)) {
		c++;
	}
	it_i("rbtree_delete() should work", c, count - delete_count);

	rbtree_destroy(&tree);
	c = 0;
	for (node = rbtree_get_min(tree.root); node; node = rbtree_next(node)) {
		c++;
	}
	it_i("rbtree_destroy() should work", c, 0);
}