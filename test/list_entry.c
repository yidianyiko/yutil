#include "../include/yutil/list_entry.h"

struct __demo_list_entry_t {
	list_entry_t entry_node;
	size_t data;
};

typedef struct __demo_list_entry_t demo_list_entry_t;

int demo_list_entry_main(int argc, char** argv)
{
	demo_list_entry_t entries[8] = { { { 0 }, 0 }, { { 0 }, 1 },
					 { { 0 }, 2 }, { { 0 }, 3 },
					 { { 0 }, 4 }, { { 0 }, 5 },
					 { { 0 }, 6 }, { { 0 }, 7 } };

	list_entry_head_t list;
	list_entry_head_init(&list, demo_list_entry_t, entry_node);

	list_entry_add_head(&list, &entries[3].entry_node);
	list_entry_add_head(&list, &entries[2].entry_node);
	list_entry_add_head(&list, &entries[1].entry_node);
	list_entry_add_tail(&list, &entries[4].entry_node);
	list_entry_add_tail(&list, &entries[5].entry_node);
	list_entry_add_tail(&list, &entries[6].entry_node);

	// get entry
	demo_list_entry_t* node =
	    list_entry(&list, &entries[0].entry_node, demo_list_entry_t);

	printf("\nadd: %lu", list_entry_size(&list));

	printf("\nentry_data:%lu", node->data);

	printf("\list_entry_get_last_entry:%lu\n",
	       list_entry_get_last_entry(&list, demo_list_entry_t)->data);

	printf("\list_entry_get_first_entry:%lu\n",
	       list_entry_get_first_entry(&list, demo_list_entry_t)->data);

	list_entry_t* entry;
	int i = 1;
	list_entry_for_each(&list, entry)
	{
		node = list_entry(&list, entry, demo_list_entry_t);

		printf("\nentry_data[%d]:%lu\n", i, node->data);
		i++;
	}

	// repalce entries
	list_entry_replace_head(&list, &entries[0].entry_node);
	list_entry_replace_last(&list, &entries[7].entry_node);
	printf("\nreplace: %lu", list_entry_size(&list));

	i = 6;
	list_entry_for_each_reverse(&list, entry)
	{
		node = list_entry(&list, entry, demo_list_entry_t);

		printf("\nentry_data[%d]:%lu\n", i, node->data);
		i--;
	}

	// delete entries
	list_entry_delete_head(&list);
	list_entry_delete_last(&list);

	printf("\ndelete: %lu", list_entry_size(&list));

	i = 1;
	list_entry_for_each_entry(&list, node, entry_node, demo_list_entry_t)
	{
		printf("\nentry_data[%d]:%lu\n", i, node->data);
		i++;
	}

	list_entry_t* head = list_entry_get_head(&list);
	list_entry_move_head(&list, list_entry_get_last(&list));
	list_entry_move_tail(&list, head);

	printf("move: %lu", list_entry_size(&list));

	i = 6;
	list_entry_for_each_entry_reverse(&list, node, entry_node,
					  demo_list_entry_t)
	{
		printf("\nentry_data[%d]:%lu\n", i, node->data);
		i--;
	}

	// clear entries
	list_entry_clear(&list);
	printf("clear: %lu", list_entry_size(&list));

	list_entry_exit(&list);

	return 0;
}

int main()
{
	demo_list_entry_main(0, 0);
	return 0;
}