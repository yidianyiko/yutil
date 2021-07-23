#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>

#include "../include/dict.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define DICTIONARY_HT_INITIAL_SIZE 4

#define time_in_milliseconds LCUI_getTime

#define DICT_STATS_VECTLEN 50
struct dictionary_item_t {
	void *key;
	union {
		void *val;
		uint64_t u64;
		int64_t s64;
	} v;
	struct dictionary_item_t *next; /**< 指向下一个哈希节点(形成链表) */
};

struct dictionary_type_t {
	unsigned int(*hashFunction)(const void *key);
	void *(*keyDup)(void *privdata, const void *key);
	void *(*valDup)(void *privdata, const void *obj);
	int(*keyCompare)(void *privdata, const void *key1, const void *key2);
	void(*keyDestructor)(void *privdata, void *key);
	void(*valDestructor)(void *privdata, void *obj);
};

struct dictionary_hash_map_t {
	dictionary_item_t **table;	/**< 节点指针数组 */
	unsigned long size;	/**< 桶的数量 */
	unsigned long sizemask;	/**< mask 码，用于地址索引计算 */
	unsigned long used;	/**< 已有节点数量 */
};

struct dictionary_t {
	dictionary_type_t *type;		/**< 为哈希表中不同类型的值所使用的一族函数 */
	void *privdata;
	dictionary_hash_map_t ht[2];	/**< 每个字典使用两个哈希表 */
	int rehashidx;		/**< rehash 进行到的索引位置，如果没有在 rehash ，就为 -1 */
	int iterators;		/**< 当前正在使用的 iterator 的数量 */
};

struct dictionary_iterator_t {
	dictionary_t *d;		/**< 迭代器所指向的字典 */
	int table;		/**< 使用的哈希表号码 */
	int index;		/**< 迭代进行的索引 */
	int safe;		/**< 是否安全 */
	dictionary_item_t *entry;	/**< 指向哈希表的当前节点 */
	dictionary_item_t *next_entry;	/**< 指向哈希表的下个节点 */
};

/* Using dictionary_enableResize() / dictionary_DisableResize() we make possible to
 * enable/disable resizing of the hash table as needed. This is very important
 * for Redis, as we use copy-on-write and don't want to move too much memory
 * around when there is a child performing saving operations.
 *
 * Note that even when dict_can_resize is set to 0, not all resizes are
 * prevented: an hash table is still allowed to grow if the ratio between
 * the number of elements and the buckets > dict_force_resize_ratio. */
static int dict_can_resize = 1;
static unsigned int dict_force_resize_ratio = 5;

static int dict_hash_function_seed = 5381;
/* -------------------------- private prototypes ---------------------------- */

static int dictionary_expand_if_needed(dictionary_t *ht);
static unsigned long dictionary_next_power(unsigned long size);
static int dictionary_key_index(dictionary_t *ht, const void *key);
static int dictionary_init(dictionary_t *ht, dictionary_type_t *type, void *privdata);

/* -------------------------- hash functions -------------------------------- */

unsigned int dictionary_make_int_hash(unsigned int key)
{
	key += ~(key << 15);
	key ^= (key >> 10);
	key += (key << 3);
	key ^= (key >> 6);
	key += ~(key << 11);
	key ^= (key >> 16);
	return key;
}

/* Identity hash function for integer keys */
unsigned int dictionary_identity_hash(unsigned int key)
{
	return key;
}


void dictionary_set_hash_seed(unsigned int seed)
{
	dict_hash_function_seed = seed;
}

unsigned int dictionary_get_hash_seed(void)
{
	return dict_hash_function_seed;
}

/* Generic hash function (a popular one from Bernstein).
 * I tested a few and this was the best. */
unsigned int dictionary_make_hash(const unsigned char *buf, int len)
{
	unsigned int hash = dict_hash_function_seed;
	while (len--) {
		hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
	}
	return hash;
}

/* And a case insensitive version */
unsigned int dictionary_make_case_hash(const unsigned char *buf, int len)
{
	unsigned int hash = dict_hash_function_seed;
	while (len--) {
		hash = ((hash << 5) + hash) +
		       (tolower(*buf++)); /* hash * 33 + c */
	}
	return hash;
}

/* ----------------------------- API implementation ------------------------- */

/** 重置哈希表 */
static void dictionary_reset(dictionary_hash_map_t *ht)
{
	ht->table = NULL;
	ht->size = 0;
	ht->sizemask = 0;
	ht->used = 0;
}

dictionary_t *dictionary_create(dictionary_type_t *type, void *privdata)
{
	static int inited = 0;
	dictionary_t *d = malloc(sizeof(dictionary_t));
	dictionary_init(d, type, privdata);
	if (!inited) {
		srand((unsigned int)time(NULL));
		inited = 1;
	}
	return d;
}

/** 初始化字典 */
static int dictionary_init(dictionary_t *d, dictionary_type_t *type, void *privdata)
{
	d->type = type;
	d->iterators = 0;
	d->rehashidx = -1;
	d->privdata = privdata;
	dictionary_reset(&d->ht[0]);
	dictionary_reset(&d->ht[1]);
	return 0;
}

int dictionary_resize(dictionary_t *d)
{
	int minimal;
	if (!dict_can_resize || dictionary_is_rehashing(d)) {
		return -1;
	}
	minimal = d->ht[0].used;
	if (minimal < DICTIONARY_HT_INITIAL_SIZE) {
		minimal = DICTIONARY_HT_INITIAL_SIZE;
	}
	return dictionary_expand(d, minimal);
}

int dictionary_expand(dictionary_t *d, unsigned long size)
{
	dictionary_hash_map_t n;
	unsigned long realsize;
	if (dictionary_is_rehashing(d) || d->ht[0].used > size) {
		return -1;
	}
	/* 计算哈希表的(真正)大小 */
	realsize = dictionary_next_power(size);
	n.used = 0;
	n.size = realsize;
	n.sizemask = realsize - 1;
	n.table = calloc(realsize, sizeof(dictionary_item_t *));
	/* 如果字典的 0 号哈希表未初始化，则将新建的哈希表作为字典的 0 号哈
	 * 希表，否则，将新建哈希表作为字典的 1 号哈希表，并将它用于 rehash
	 */
	if (d->ht[0].table == NULL) {
		d->ht[0] = n;
	} else {
		d->ht[1] = n;
		d->rehashidx = 0;
	}
	return 0;
}

int dictionary_rehash(dictionary_t *d, int n)
{
	if (!dictionary_is_rehashing(d)) {
		return 0;
	}
	while (n--) {
		dictionary_item_t *de, *nextde;
		if (d->ht[0].used == 0) {
			free(d->ht[0].table);
			d->ht[0] = d->ht[1];
			dictionary_reset(&d->ht[1]);
			d->rehashidx = -1;
			return 0;
		}
		assert(d->ht[0].size > (unsigned)d->rehashidx);
		while (d->ht[0].table[d->rehashidx] == NULL) {
			d->rehashidx++;
		}
		de = d->ht[0].table[d->rehashidx];
		while (de) {
			unsigned int h;
			nextde = de->next;
			h = dictionary_hash_key(d, de->key) & d->ht[1].sizemask;
			de->next = d->ht[1].table[h];
			d->ht[1].table[h] = de;
			d->ht[0].used--;
			d->ht[1].used++;
			de = nextde;
		}
		d->ht[0].table[d->rehashidx] = NULL;
		d->rehashidx++;
	}
	return 1;
}


int dictionary_rehash_milliseconds(dictionary_t *d, int ms)
{
	long long start = time_in_milliseconds();
	int rehashes = 0;

	while (dictionary_rehash(d, 100)) {
		rehashes += 100;
		if (time_in_milliseconds() - start > ms) {
			break;
		}
	}
	return rehashes;
}

/** 在字典没有迭代器的情况下，rehash 一个元素 */
static void dictionary_rehash_step(dictionary_t *d)
{
	if (d->iterators == 0) {
		dictionary_rehash(d, 1);
	}
}

int dictionary_add(dictionary_t *d, void *key, void *val)
{
	dictionary_item_t *entry = dictionary_add_raw(d, key);
	if (!entry) {
		return -1;
	}
	dictionary_set_val(d, entry, val);
	return 0;
}

int dictionary_add_2(dictionary_t *d, void *key, const void *val)
{
	dictionary_item_t *entry;
	if (!d->type->valDup) {
		return -2;
	}
	entry = dictionary_add_raw(d, key);
	if (!entry) {
		return -1;
	}
	entry->v.val = d->type->valDup(d->privdata, val);
	return 0;
}

dictionary_item_t *dictionary_add_raw(dictionary_t *d, void *key)
{
	int index;
	dictionary_item_t *entry;
	dictionary_hash_map_t *ht;

	/* 如果可以执行 rehash 操作，则执行平摊 rehash 操作 */
	if (dictionary_is_rehashing(d)) {
		dictionary_rehash_step(d);
	}
	/* 计算 key 的 index 值，如果 key 已经存在，dictionary_key_index 返回 -1 */
	if ((index = dictionary_key_index(d, key)) == -1) {
		return NULL;
	}
	/* 判断是否正在进行 rehash ，选择相应的表 */
	ht = dictionary_is_rehashing(d) ? &d->ht[1] : &d->ht[0];
	entry = malloc(sizeof(*entry));
	entry->next = ht->table[index];
	ht->table[index] = entry;
	ht->used++;
	dictionary_set_key(d, entry, key);
	return entry;
}

int dictionary_replace(dictionary_t *d, void *key, void *val)
{
	dictionary_item_t *entry, auxentry;
	if (dictionary_add(d, key, val) == 0) {
		return 1;
	}
	entry = dictionary_find(d, key);
	auxentry = *entry;
	dictionary_set_val(d, entry, val);
	dictionary_free_val(d, &auxentry);
	return 0;
}

dictionary_item_t *dictionary_replace_raw(dictionary_t *d, void *key)
{
	dictionary_item_t *entry = dictionary_find(d, key);
	return entry ? entry : dictionary_add_raw(d, key);
}

/* 删除字典中的指定元素 */
static int dictionary_generic_delete(dictionary_t *d, const void *key, int nofree)
{
	int table;
	unsigned int h, idx;
	dictionary_item_t *he, *prev_he;
	if (d->ht[0].size == 0) {
		return -1;
	}
	if (dictionary_is_rehashing(d)) {
		dictionary_rehash_step(d);
	}
	h = dictionary_hash_key(d, key);
	for (table = 0; table <= 1; table++) {
		idx = h & d->ht[table].sizemask;
		he = d->ht[table].table[idx];
		prev_he = NULL;
		while (he) {
			if (dictionary_compare_keys(d, key, he->key)) {
				if (prev_he) {
					prev_he->next = he->next;
				} else {
					d->ht[table].table[idx] = he->next;
				}
				if (!nofree) {
					dictionary_free_key(d, he);
					dictionary_free_val(d, he);
				}
				free(he);
				d->ht[table].used--;
				return 0;
			}
			prev_he = he;
			he = he->next;
		}
		if (!dictionary_is_rehashing(d)) {
			break;
		}
	}
	return -1;
}

int dictionary_Delete(dictionary_t *ht, const void *key)
{
	return dictionary_generic_delete(ht, key, 0);
}

int dictionary_Delete_no_free(dictionary_t *ht, const void *key)
{
	return dictionary_generic_delete(ht, key, 1);
}

/** 清除字典中指定的哈希表 */
static void dictionary_clear(dictionary_t *d, dictionary_hash_map_t *ht)
{
	unsigned long i;
	dictionary_item_t *he, *next_he;
	for (i = 0; i < ht->size && ht->used > 0; i++) {
		he = ht->table[i];
		while (he) {
			next_he = he->next;
			dictionary_free_key(d, he);
			dictionary_free_val(d, he);
			free(he);
			ht->used--;
			he = next_he;
		}
	}
	free(ht->table);
	dictionary_reset(ht);
}

void dictionary_release(dictionary_t *d)
{
	dictionary_clear(d, &d->ht[0]);
	dictionary_clear(d, &d->ht[1]);
	free(d);
}

dictionary_item_t *dictionary_find(dictionary_t *d, const void *key)
{
	dictionary_item_t *he;
	unsigned int h, idx, table;
	if (d->ht[0].size == 0) {
		return NULL;
	}
	if (dictionary_is_rehashing(d)) {
		dictionary_rehash_step(d);
	}
	h = dictionary_hash_key(d, key);
	for (table = 0; table <= 1; table++) {
		idx = h & d->ht[table].sizemask;
		he = d->ht[table].table[idx];
		while (he) {
			if (dictionary_compare_keys(d, key, he->key)) {
				return he;
			}
			he = he->next;
		}
		if (!dictionary_is_rehashing(d)) {
			return NULL;
		}
	}
	return NULL;
}

void *dictionary_fetch_value(dictionary_t *d, const void *key)
{
	dictionary_item_t *he = dictionary_find(d, key);
	return he ? dictionary_item_get_val(he) : NULL;
}

dictionary_iterator_t *dictionary_get_iterator(dictionary_t *d)
{
	dictionary_iterator_t *iter = malloc(sizeof(*iter));
	iter->d = d;
	iter->table = 0;
	iter->index = -1;
	iter->safe = 0;
	iter->entry = NULL;
	iter->next_entry = NULL;
	return iter;
}

dictionary_iterator_t *dictionary_get_safe_iterator(dictionary_t *d)
{
	dictionary_iterator_t *i = dictionary_get_iterator(d);
	i->safe = 1;
	return i;
}

dictionary_item_t *dictionary_next_iterator(dictionary_iterator_t *iter)
{
	dictionary_hash_map_t *ht;
	while (1) {
		if (iter->entry) {
			iter->entry = iter->next_entry;
			if (iter->entry) {
				iter->next_entry = iter->entry->next;
				return iter->entry;
			}
			continue;
		}
		/* 如果迭代器是新的(未使用过)，那么初始化新迭代器 */
		ht = &iter->d->ht[iter->table];
		if (iter->safe && iter->index == -1 && iter->table == 0) {
			iter->d->iterators++;
		}
		iter->index++;
		if (iter->index >= (signed)ht->size) {
			if (dictionary_is_rehashing(iter->d) && iter->table == 0) {
				iter->table++;
				iter->index = 0;
				ht = &iter->d->ht[1];
			} else {
				break;
			}
		}
		iter->entry = ht->table[iter->index];
		if (iter->entry) {
			iter->next_entry = iter->entry->next;
			return iter->entry;
		}
	}
	return NULL;
}

void dictionary_release_iterator(dictionary_iterator_t *iter)
{
	if (iter->safe && !(iter->index == -1 && iter->table == 0)) {
		iter->d->iterators--;
	}
	free(iter);
}

dictionary_item_t *dictionary_get_random_key(dictionary_t *d)
{
	dictionary_item_t *he, *orighe;
	unsigned int h;
	int listlen, listele;

	if (dictionary_Size(d) == 0) {
		return NULL;
	}
	if (dictionary_is_rehashing(d)) {
		dictionary_rehash_step(d);
	}
	if (dictionary_is_rehashing(d)) {
		do {
			h = rand() % (d->ht[0].size + d->ht[1].size);
			if (h >= d->ht[0].size) {
				he = d->ht[1].table[h - d->ht[0].size];
			} else {
				he = d->ht[0].table[h];
			}
		} while (he == NULL);
	} else {
		do {
			h = rand() & d->ht[0].sizemask;
			he = d->ht[0].table[h];
		} while (he == NULL);
	}
	listlen = 0;
	orighe = he;
	while (he) {
		he = he->next;
		listlen++;
	}
	/* 从链表里随机取一项 */
	listele = rand() % listlen;
	he = orighe;
	while (listele--) he = he->next;
	return he;
}

/* ------------------------- private functions ------------------------------ */

/* Expand the hash table if needed */
static int dictionary_expand_if_needed(dictionary_t *d)
{
	/* Incremental rehashing already in progress. Return. */
	if (dictionary_is_rehashing(d)) {
		return 0;
	}
	/* If the hash table is empty expand it to the intial size. */
	if (d->ht[0].size == 0) {
		return dictionary_expand(d, DICTIONARY_HT_INITIAL_SIZE);
	}
	/* If we reached the 1:1 ratio, and we are allowed to resize the hash
	 * table (global setting) or we should avoid it but the ratio between
	 * elements/buckets is over the "safe" threshold, we resize doubling
	 * the number of buckets. */
	if (d->ht[0].used >= d->ht[0].size &&
	    (dict_can_resize ||
	     d->ht[0].used / d->ht[0].size > dict_force_resize_ratio)) {
		return dictionary_expand(
		    d, ((d->ht[0].size > d->ht[0].used) ? d->ht[0].size
							: d->ht[0].used) *
			   2);
	}
	return 0;
}

static unsigned long dictionary_next_power(unsigned long size)
{
	unsigned long i = DICTIONARY_HT_INITIAL_SIZE;

	if (size >= LONG_MAX)
		return LONG_MAX;
	while (1) {
		if (i >= size)
			return i;
		i *= 2;
	}
}

/**
 * 获取字典中与 key 对应的空槽的索引
 * @returns 如果 key 已经存在，则返回 -1
 */
static int dictionary_key_index(dictionary_t *d, const void *key)
{
	dictionary_item_t *he;
	unsigned int h, idx, table;
	if (dictionary_expandif_needed(d) == -1) {
		return -1;
	}
	h = dictionary_hash_key(d, key);
	for (table = 0; table <= 1; table++) {
		idx = h & d->ht[table].sizemask;
		he = d->ht[table].table[idx];
		while (he) {
			if (dictionary_compare_keys(d, key, he->key)) {
				return -1;
			}
			he = he->next;
		}
		if (!dictionary_is_rehashing(d)) {
			break;
		}
	}
	return idx;
}

void dictionary_empty(dictionary_t *d)
{
	dictionary_clear(d, &d->ht[0]);
	dictionary_clear(d, &d->ht[1]);
	d->rehashidx = -1;
	d->iterators = 0;
}

static void dictionary_print_stats_Ht(dictionary_hash_map_t *ht)
{
	unsigned long totchainlen = 0;
	unsigned long clvector[DICT_STATS_VECTLEN];
	unsigned long i, slots = 0, chainlen, maxchainlen = 0;
	if (ht->used == 0) {
		Logger_info("No stats available for empty dictionaries\n");
		return;
	}
	for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
	for (i = 0; i < ht->size; i++) {
		dictionary_item_t *he;
		if (ht->table[i] == NULL) {
			clvector[0]++;
			continue;
		}
		slots++;
		chainlen = 0;
		he = ht->table[i];
		while (he) {
			chainlen++;
			he = he->next;
		}
		if (chainlen < DICT_STATS_VECTLEN) {
			clvector[chainlen]++;
		} else {
			clvector[DICT_STATS_VECTLEN - 1]++;
		}
		if (chainlen > maxchainlen) {
			maxchainlen = chainlen;
		}
		totchainlen += chainlen;
	}
	printf("Hash table stats:\n");
	printf(" table size: %ld\n", ht->size);
	printf(" number of elements: %ld\n", ht->used);
	printf(" different slots: %ld\n", slots);
	printf(" max chain length: %ld\n", maxchainlen);
	printf(" avg chain length (counted): %.02f\n",
	       (float)totchainlen / slots);
	printf(" avg chain length (computed): %.02f\n",
	       (float)ht->used / slots);
	printf(" Chain length distribution:\n");
	for (i = 0; i < DICT_STATS_VECTLEN - 1; i++) {
		if (clvector[i] == 0) {
			continue;
		}
		printf("   %s%ld: %ld (%.02f%%)\n",
		       (i == DICT_STATS_VECTLEN - 1) ? ">= " : "", i,
		       clvector[i], ((float)clvector[i] / ht->size) * 100);
	}
}

void dictionary_print_stats(dictionary_t *d)
{
	dictionary_print_stats_Ht(&d->ht[0]);
	if (dictionary_is_rehashing(d)) {
		printf("-- _rehashing into ht[1]:\n");
		dictionary_print_stats_Ht(&d->ht[1]);
	}
}

void dictionary_enable_resize(void)
{
	dict_can_resize = 1;
}

void dictionary_disable_resize(void)
{
	dict_can_resize = 0;
}

unsigned int string_key_dictionary_get_key_hash(const void *key)
{
	const char *buf = key;
	unsigned int hash = dict_hash_function_seed;
	while (*buf) {
		hash = ((hash << 5) + hash) + (*buf++);
	}
	return hash;
}

int string_key_dictionary_compare_key(void *privdata, const void *key1,
					const void *key2)
{
	if (strcmp(key1, key2) == 0) {
		return 1;
	}
	return 0;
}

void *string_key_dictionary_duplicate_key(void *privdata, const void *key)
{
	char *newkey = malloc((strlen(key) + 1) * sizeof(char));
	strcpy(newkey, key);
	return newkey;
}

void string_key_dictionary_destroy_key(void *privdata, void *key)
{
	free(key);
}

void dictionary_init_string_key_type(dictionary_type_t *t)
{
	t->hashFunction = string_key_dictionary_get_key_hash;
	t->keyDup = NULL;
	t->valDup = NULL;
	t->keyCompare = string_key_dictionary_compare_key;
	t->keyDestructor = NULL;
	t->valDestructor = NULL;
}

void dictionary_init_string_key_type_copy(dictionary_type_t *t)
{
	t->hashFunction = string_key_dictionary_get_key_hash;
	t->keyDup = string_key_dictionary_duplicate_key;
	t->valDup = NULL;
	t->keyCompare = string_key_dictionary_compare_key;
	t->keyDestructor = string_key_dictionary_destroy_key;
	t->valDestructor = NULL;
}
