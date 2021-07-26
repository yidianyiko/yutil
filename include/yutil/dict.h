/* Hash Tables Implementation.
 *
 * This file implements in-memory hash tables with insert/del/replace/find/
 * get-random-element operations. Hash tables will auto-resize if needed
 * tables of power of two in size are used, collisions are handled by
 * chaining. See the source code for more information... :)
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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
 *   * Neither the name of Redis nor the names of its contributors may be used
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

#ifndef UTIL_DICT_H
#define UTIL_DICT_H

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

/* ------------------------------- Types ------------------------------------*/
/** the dictionary entry type */
typedef struct dict_entry_t dict_entry_t;

/** the data in dictionary type */
typedef struct dict_type_t dict_type_t;

/** the dictionary hash map type */
typedef struct dict_hash_map_t dict_hash_map_t;

/** the dictionary type */
typedef struct dict_t dict_t;

/** the dictionary safe iterator type */
typedef struct dict_iterator_t dict_iterator_t;

typedef void(dict_scan_function)(void *priv_data, const dict_entry_t *de);
typedef void(dict_scan_bucket_function)(void *priv_data,
					dict_entry_t **bucket_ref);

/* ------------------------------- Macros ------------------------------------*/
#define DICT_OK 0
#define DICT_ERR 1

#define dict_free_val(d, entry)        \
	if ((d)->type->val_destructor) \
	(d)->type->val_destructor((d)->priv_data, (entry)->v.val)

#define dict_set_val(d, entry, _val_)                                  \
	do {                                                           \
		if ((d)->type->val_dup)                                \
			(entry)->v.val =                               \
			    (d)->type->val_dup((d)->priv_data, _val_); \
		else                                                   \
			(entry)->v.val = (_val_);                      \
	} while (0)

#define dict_set_signed_integer_val(entry, _val_) \
	do {                                      \
		(entry)->v.s64 = _val_;           \
	} while (0)

#define dict_set_unsigned_integer_val(entry, _val_) \
	do {                                        \
		(entry)->v.u64 = _val_;             \
	} while (0)

#define dict_set_double_val(entry, _val_) \
	do {                              \
		(entry)->v.d = _val_;     \
	} while (0)

#define dict_free_key(d, entry)        \
	if ((d)->type->key_destructor) \
	(d)->type->key_destructor((d)->priv_data, (entry)->key)

#define dict_set_key(d, entry, _key_)                                  \
	do {                                                           \
		if ((d)->type->key_dup)                                \
			(entry)->key =                                 \
			    (d)->type->key_dup((d)->priv_data, _key_); \
		else                                                   \
			(entry)->key = (_key_);                        \
	} while (0)

#define dict_compare_keys(d, key1, key2)                          \
	(((d)->type->key_compare)                                 \
	     ? (d)->type->key_compare((d)->priv_data, key1, key2) \
	     : (key1) == (key2))

#define dict_hash_key(d, key) (d)->type->hash_function(key)
#define dict_get_key(he) ((he)->key)
#define dict_get_val(he) ((he)->v.val)
#define dict_get_signed_integer_val(he) ((he)->v.s64)
#define dict_get_unsigned_integer_val(he) ((he)->v.u64)
#define dict_get_double_val(he) ((he)->v.d)
#define dict_slots(d) ((d)->ht[0].size + (d)->ht[1].size)
#define dict_size(d) ((d)->ht[0].used + (d)->ht[1].used)
#define dict_is_rehashing(d) ((d)->rehash_index != -1)
#define dict_pause_rehashing(d) (d)->pause_rehash++
#define dict_resume_rehashing(d) (d)->pause_rehash--

/* If our unsigned long type can store a 64 bit number, use a 64 bit PRNG. */
#if ULONG_MAX >= 0xffffffffffffffff
unsigned long long genrand64_int64(void);
#define random_u_long() ((unsigned long)gen_rand64_int64())
#else
#define random_u_long() rand()
#endif

/* API */
dict_t *dict_create(dict_type_t *type, void *priv_data_ptr);
int dict_expand(dict_t *d, unsigned long size);
int dict_try_expand(dict_t *d, unsigned long size);
int dict_add(dict_t *d, void *key, void *val);
dict_entry_t *dict_add_raw(dict_t *d, void *key, dict_entry_t **existing);
dict_entry_t *dict_add_or_find(dict_t *d, void *key);
int dict_replace(dict_t *d, void *key, void *val);
int dict_delete(dict_t *d, const void *key);
dict_entry_t *dict_unlink(dict_t *ht, const void *key);
void dict_free_unlinked_entry(dict_t *d, dict_entry_t *he);
void dict_release(dict_t *d);
dict_entry_t *dict_find(dict_t *d, const void *key);
void *dict_fetch_value(dict_t *d, const void *key);
int dict_resize(dict_t *d);
dict_iterator_t *dict_get_iterator(dict_t *d);
dict_iterator_t *dict_get_safe_iterator(dict_t *d);
dict_entry_t *dict_next(dict_iterator_t *iter);
void dict_release_iterator(dict_iterator_t *iter);
dict_entry_t *dict_get_random_key(dict_t *d);
dict_entry_t *dict_get_fair_random_key(dict_t *d);
unsigned int dict_get_some_keys(dict_t *d, dict_entry_t **des,
				unsigned int count);
void dict_get_stats(char *buf, size_t buf_size, dict_t *d);
unsigned int dict_gen_hash_function(const unsigned char *buf, int len);
unsigned int dict_gen_case_hash_function(const unsigned char *buf, int len);
void dict_empty(dict_t *d, void(callback)(void *));
void dict_enable_resize(void);
void dict_disable_resize(void);
int dict_rehash(dict_t *d, int n);
int dict_rehash_milliseconds(dict_t *d, int ms);
void dict_set_hash_function_seed(unsigned int *seed);
unsigned int dict_get_hash_function_seed(void);
unsigned long dict_scan(dict_t *d, unsigned long v, dict_scan_function *fn,
			dict_scan_bucket_function *bucketfn, void *priv_data);
uint64_t dict_get_hash(dict_t *d, const void *key);
dict_entry_t **dict_find_entry_ref_by_ptr_and_hash(dict_t *d,
						   const void *old_ptr,
						   uint64_t hash);

#endif /* UTIL_DICT_H */