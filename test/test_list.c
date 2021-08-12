#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include "libtest.h"
#include "../include/yutil/list.h"

void test_list_concat(void)
{
	list_t list1;
	list_t list2;
	list_init(&list1);
	list_init(&list2);
	list_append(&list1, 0);
	list_append(&list2, 10);
	list_concat(&list1, &list2);

	it_b("list_concat() should work",
	     (int)list_get_size(&list1) == 2 &&
		 (int)list_get_size(&list2) == 0 && !list2.head.next &&
		 !list2.tail.prev,
	     TRUE);
	list_clear(&list1, NULL);
	list_clear(&list2, NULL);
}

void test_list(void)
{
	int arr[] = { 0, 4, 8, 16, 32, 64, 1024 };
	size_t i;
	size_t n = sizeof(arr) / sizeof(int);

	list_t list;
	list_t list2;
	list_node_t *node;

	list_init(&list);
	it_b("list_init() should work",
	     (int)list_get_size(&list) == 0 && !list.head.next &&
		 !list.tail.prev,
	     TRUE);

	// append data
	for (i = 0; i < n; ++i) {
		list_append(&list, arr + i);
	}
	it_i("list_append() should work", (int)list_get_size(&list), (int)n);
	it_i("list_append_node() should work", (int)list_get_size(&list),
	     (int)n);

	// delete data
	for (i = 0; i < n; ++i) {
		list_delete_by_pos(&list, 0);
	}
	it_i("list_delete_by_pos() should work", (int)list_get_size(&list),
	     (int)0);
	it_i("list_delete_node() should work", (int)list_get_size(&list),
	     (int)0);
	it_i("list_remove_link() should work", (int)list_get_size(&list),
	     (int)0);
	it_i("list_node_free() should work", (int)list_get_size(&list), (int)0);
	// insert data
	for (i = 0; i < n; ++i) {
		list_insert(&list, i, arr + i);
	}
	it_i("list_insert() should work", (int)list_get_size(&list), (int)n);
	it_i("list_insert_node() should work", (int)list_get_size(&list),
	     (int)n);
	it_i("list_add_link() should work", (int)list_get_size(&list), (int)n);

	// insert head
	list_insert_head(&list, 0);
	it_i("list_insert_head() should work", (int)list_get_size(&list),
	     (int)n + 1);
	// delete head
	list_delete_head(&list);
	it_i("list_delete_head() should work", (int)list_get_size(&list),
	     (int)n);

	list_append(&list, 0);

	// delete tail
	list_delete_tail(&list);
	it_i("list_delete_tail() should work", (int)list_get_size(&list),
	     (int)n);

	// list_for_each
	i = 0;
	list_for_each(node, &list)
	{
		if (node->data != arr + i) {
			break;
		}
		++i;
	}
	it_b("list_for_each() should work",
	     node == NULL && i == sizeof(arr) / sizeof(int), TRUE);

	// list_for_each_reverse
	i = n - 1;
	list_for_each_reverse(node, &list)
	{
		if (node->data != arr + i) {
			break;
		}
		if (i == 0) {
			i = n;
			break;
		}
		--i;
	}
	it_i("list_for_each_reverse() should work", (int)i, (int)n);

	it_i("list_get_size() should work", (int)list_get_size(&list), (int)n);

	it_b("list_get() should work", list_get(&list, 3) == (arr + 3), TRUE);

	it_b("list_get_node_at_head() should work",
	     list_get_node_at_head(&list)->data == (arr), TRUE);
	it_b("list_get_node_at_tail() should work",
	     list_get_node_at_tail(&list)->data == (arr + n - 1), TRUE);

	test_list_concat();

	list_clear(&list, NULL);
	it_b("list_clear() should work",
	     (int)list_get_size(&list) == 0 && !list.head.next &&
		 !list.tail.prev,
	     TRUE);
}
