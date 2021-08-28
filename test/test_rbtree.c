#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/keywords.h"
#include "../include/yutil/rbtree.h"

int main(int argc, char const* argv[])
{
	const int delete_cnt = 1024;
	double duration;
	double room;

	rbtree_t t = { 0 };
	rbtree_node_t s = { 0 };
	rbtree_init(&t, &s);
	// rbtree_set_destroy_func(&t,free);
	const int cnt = 1 << 20;
	const int max_len = 15;

#define TEST_VALUES                                                     \
	{                                                               \
		"apple", "banana", "cherry", "grape", "lemon", "mango", \
		    "pear", "pineapple", "strawberry", "watermelon"     \
	}

	/* for gcc */
	char* v[] = TEST_VALUES;
	/* for g++ */
	// char v[][max_len] = TEST_VALUES;

	/* Default stack size in Ubuntu Kylin 14.04 is 8MB. */
	/* It's not enough. So I use memory in heap which offers a lot larger
	 * room. */
	rbtree_node_t* n = (rbtree_node_t*)calloc(cnt, sizeof(rbtree_node_t));
	int i;

	long time1 = clock();
	printf("\nInsert:\n");
	for (i = 0; i < cnt; i++) {
		n[i].key = i + 1;
		n[i].data = v[i % 10];
		rbtree_insert(&t, &n[i]);
	}

	long time2 = clock();
	room = 48.0 * cnt / (1 << 20);
	duration = (double)(time2 - time1) / CLOCKS_PER_SEC;
	printf("Inserting %d nodes costs %.2fMB and spends %f seconds.\n", cnt,
	       room, duration);
	rbtree_node_t* node;
	int c = 0;
	for (node = rbtree_get_min(&t, t.root); node;
	     node = rbtree_next(&t, node)) {
		printf("\n%d\n", c++);
	}
	const int search_cnt = 1 << 10;
	srand((unsigned int)time(0));
	for (i = 0; i < search_cnt; i++) {
		rbtree_search_by_key(&t, (rand() % cnt) + 1);
	}

	long time3 = clock();
	duration = (double)(time3 - time2) / CLOCKS_PER_SEC;
	printf("Searching %d nodes among %d spends %f seconds.\n", search_cnt,
	       cnt, duration);

	int nums[1024];
	int num;
	/* Let's hash! */
	char* mark = (char*)calloc(cnt, sizeof(char));
	memset(mark, 0, cnt * sizeof(char));
	for (i = 0; i < delete_cnt; i++) {
		for (;;) {
			num = rand() % cnt;
			if (mark[num] == 0) {
				mark[num] = 1;
				nums[i] = num;
				break;
			}
		}
	}

	long time4 = clock();
	duration = (double)(time4 - time3) / CLOCKS_PER_SEC;
	printf("Hash %d times spends %f seconds.\n", delete_cnt, duration);

	for (i = 0; i < delete_cnt; i++) {
		rbtree_delete(&t, &n[nums[i]]);
	}

	long time5 = clock();
	duration = (double)(time5 - time4) / CLOCKS_PER_SEC;
	printf("Deleting %d nodes among %d spends %f seconds.\n", delete_cnt,
	       cnt, duration);
	free(mark);
	free(n);

	return 0;
}