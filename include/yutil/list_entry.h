#ifndef UTIL_LIST_H
#define UTIL_LIST_H

/* ------------------------------- includes --------------------------------*/
#include "types.h"

/* ------------------------------- types -----------------------------------*/

// the doubly-linked list entry type
struct __list_entry_t {
	struct __list_entry_t* next;
	struct __list_entry_t* prev;
};

// the list entry head type
struct __list_entry_head_t {
	struct __list_entry_t* next;
	struct __list_entry_t* prev;
	size_t length;
	size_t entry_offset;
};

typedef struct __list_entry_t list_entry_t;
typedef struct __list_entry_head_t list_entry_head_t;

/* ------------------------------- macros ------------------------------------*/

#define offsetof(type, member) ((size_t) & ((type*)0)->member)

#define list_entry_head_init(list, type, member) \
	list_entry_head_init_(list, offsetof(type, member))

#define list_entry_clear(list) list_entry_head_init_(list, 0)

// get the struct for this entry
#define list_entry(head, entry, type) \
	((type*)((unsigned char*)(entry) - (head)->entry_offset))

#define list_entry_get_first_entry(head, type) \
	list_entry(head, (head)->next, type)

#define list_entry_get_last_entry(head, type) \
	list_entry(head, (head)->prev, type)

#define list_entry_next_entry_(head, node, member, type) \
	list_entry(head, (node)->member.next, type)

#define list_entry_prev_entry_(head, node, member, type) \
	list_entry(head, (node)->member.prev, type)

#define list_entry_for_each(head, entry)                           \
	for (entry = (head)->next; entry != (list_entry_t*)(head); \
	     entry = entry->next)

#define list_entry_for_each_reverse(head, entry)                   \
	for (entry = (head)->prev; entry != (list_entry_t*)(head); \
	     entry = entry->prev)

#define list_entry_for_each_entry(head, node, member, type) \
	for (node = list_entry_get_first_entry(head, type); \
	     &node->member != (list_entry_t*)(&list);       \
	     node = list_entry_next_entry_(head, node, member, type))

#define list_entry_for_each_entry_reverse(head, node, member, type) \
	for (node = list_entry_get_last_entry(head, type);          \
	     &node->member != (list_entry_t*)(&list);               \
	     node = list_entry_prev_entry_(head, node, member, type))

// init list entry
static inline void list_entry_init(list_entry_t* entry)
{
	entry->next = entry;
	entry->prev = entry;
}

// init list entry head
static inline void list_entry_head_init_(list_entry_head_t* list,
					 size_t entry_offset)
{
	list->next = (list_entry_t*)list;
	list->prev = (list_entry_t*)list;
	list->length = 0;
	list->entry_offset = entry_offset;
}

// destory
static inline void list_entry_exit(list_entry_head_t* head)
{
	head->next = NULL;
	head->prev = NULL;
	head->length = 0;
	head->entry_offset = 0;
}

// the list entry length
static inline size_t list_entry_size(list_entry_head_t* head)
{
	return head->length;
}

// the list head entry
static inline list_entry_t* list_entry_get_head(list_entry_head_t* head)
{
	return head->next;
}

// the list last entry
static inline list_entry_t* list_entry_get_last(list_entry_head_t* head)
{
	return head->prev;
}

// the list is empty?
static inline bool_t list_entry_is_empty(list_entry_head_t* head)
{
	return !(head->length);
}

// is the list head entry?
static inline bool_t list_entry_is_head(list_entry_head_t* head,
					list_entry_t* entry)
{
	return head->next == entry;
}

// is the list last entry?
static inline bool_t list_entry_is_last(list_entry_head_t* head,
					list_entry_t* entry)
{
	return head->prev == entry;
}

// a list has just one entry?
static inline int list_is_singular(list_entry_head_t* head)
{
	return !list_entry_is_empty(head) && (head->length == 1);
}

static inline bool_t list_entry_is_valid(list_entry_head_t* list)
{
	return (list->next && list->next->prev == (list_entry_t*)list) &&
	       (list->prev && list->prev->next == (list_entry_t*)list);
}

// Insert a new entry after node.
static inline void list_entry_add_next(list_entry_head_t* head,
				       list_entry_t* node, list_entry_t* entry)
{
	assert(head && node && node->next && entry);
	assert(node != entry);
	assert(list_entry_is_valid(head));

	node->next->prev = entry;
	entry->next = node->next;
	entry->prev = node;
	node->next = entry;

	head->length++;
}

// Insert entry to the prev
#define list_entry_add_prev(head, node, entry) \
	list_entry_add_next(head, ((node)->prev), entry)

// Insert entry to the head
static inline void list_entry_add_head(list_entry_head_t* head,
				       list_entry_t* entry)
{
	list_entry_add_next(head, (list_entry_t*)head, entry);
}

// Insert entry to the tail
static inline void list_entry_add_tail(list_entry_head_t* head,
				       list_entry_t* entry)
{
	list_entry_add_next(head, head->prev, entry);
}

// delete the entry (private interface)
static inline void list_entry_delete_(list_entry_head_t* head,
				      list_entry_t* prev, list_entry_t* next)
{
	assert(head && head->length && prev && next);

	assert(list_entry_is_valid(head));

	prev->next = next;
	next->prev = prev;

	head->length--;
}

// delete the entry
static inline void list_entry_delete(list_entry_head_t* head,
				     list_entry_t* entry)
{
	assert(entry);

	list_entry_delete_(head, entry->prev, entry->next);
}

// delete the entry and reinitialize it.
static inline void list_entry_delete_init(list_entry_head_t* head,
					  list_entry_t* entry)
{
	assert(entry);

	list_entry_delete_(head, entry->prev, entry->next);
	list_entry_init(entry);
}

// delete the next entry
static inline void list_entry_delete_next(list_entry_head_t* head,
					  list_entry_t* prev)
{
	assert(prev && prev->next);

	list_entry_delete_(head, prev, prev->next->next);
}

// delete the prev entry
static inline void list_entry_delete_prev(list_entry_head_t* head,
					  list_entry_t* next)
{
	assert(next && next->prev);

	list_entry_delete_(head, next->prev->prev, next);
}

// delete the head entry
static inline void list_entry_delete_head(list_entry_head_t* head)
{
	assert(head && head->next);

	list_entry_delete_(head, (list_entry_t*)head, head->next->next);
}

// delete the last entry
static inline void list_entry_delete_last(list_entry_head_t* head)
{
	assert(head && head->prev);

	list_entry_delete_(head, head->prev->prev, (list_entry_t*)head);
}

// delete from one list and add as another's head
static inline void list_entry_move_next(list_entry_head_t* head,
					list_entry_t* node, list_entry_t* entry)
{
	list_entry_delete(head, entry);
	list_entry_add_next(head, node, entry);
}

// move the prev entry
static inline void list_entry_move_prev(list_entry_head_t* head,
					list_entry_t* node, list_entry_t* entry)
{
	assert(node);

	list_entry_move_next(head, node->prev, entry);
}

// move the head entry
static inline void list_entry_move_head(list_entry_head_t* head,
					list_entry_t* entry)
{
	list_entry_move_next(head, (list_entry_t*)head, entry);
}

// move the tail entry
static inline void list_entry_move_tail(list_entry_head_t* head,
					list_entry_t* entry)
{
	assert(head);

	list_entry_move_next(head, head->prev, entry);
}

// replace old entry by new one
static inline void list_entry_replace(list_entry_t* old_entry,
				      list_entry_t* new_entry)
{
	assert(old_entry && old_entry->next && old_entry->prev && new_entry);
	assert(old_entry != new_entry);

	new_entry->next = old_entry->next;
	new_entry->next->prev = new_entry;
	new_entry->prev = old_entry->prev;
	new_entry->prev->next = new_entry;
}

// replace the head entry
static inline void list_entry_replace_head(list_entry_head_t* head,
					   list_entry_t* entry)
{
	assert(head);

	list_entry_replace(head->next, entry);
}

// replace the last entry
static inline void list_entry_replace_last(list_entry_head_t* head,
					   list_entry_t* entry)
{
	assert(head);

	list_entry_replace(head->prev, entry);
}

// rotate the list to the left. move head to tail
static inline void list_rotate_left(list_entry_head_t* head)
{
	list_entry_t* entry;
	if (!list_entry_is_empty(head)) {
		entry = head->next;
		list_entry_move_tail(head, entry);
	}
}

// splice the spliced_list to list[prev, next]
static inline void list_entry_splice(list_entry_head_t* head,
				     list_entry_t* prev, list_entry_t* next,
				     list_entry_head_t* spliced_list)
{
	if (!list_entry_is_empty(spliced_list)) {
		spliced_list->next->prev = prev;
		prev->next = spliced_list->next;
		spliced_list->prev->next = next;
		next->prev = spliced_list->prev;

		head->length += spliced_list->length;

		list_entry_clear(spliced_list);
	}
}

// splice the spliced_list at the list head
static inline void list_entry_splice_head(list_entry_head_t* head,
					  list_entry_head_t* spliced_list)
{
	assert(head);

	list_entry_splice(head, (list_entry_t*)head, head->next, spliced_list);
}

// splice the spliced_list at the list tail
static inline void list_entry_splice_tail(list_entry_head_t* head,
					  list_entry_head_t* spliced_list)
{
	assert(head);

	list_entry_splice(head, head->prev, (list_entry_t*)head, spliced_list);
}

#endif