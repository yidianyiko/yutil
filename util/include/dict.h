#include <stdint.h>

#ifndef LCUI_UTIL_dICT_H
#define LCUI_UTIL_dICT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/** 哈希表节点结构 */
typedef struct dictionary_item_t dictionary_item_t;

/** 字典内数据的类型 */
typedef struct dictionary_type_t dictionary_type_t;

/** 哈希表结构 */
typedef struct dictionary_hash_map_t dictionary_hash_map_t;

/** 字典结构 */
typedef struct dictionary_t dictionary_t;

/** 用于遍历字典的迭代器 */
typedef struct dictionary_iterator_t dictionary_iterator_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define DICTIONARY_HT_iNITIAL__sIZE 4

#define dictionary_free_val(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

#define dictionary_set_val(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        entry->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        entry->v.val = (_val_); \
} while(0)

#define dictionary_set_signed_integer_val(entry, _val_) \
    do { entry->v.s64 = _val_; } while(0)

#define dictionary_set_unsigned_integer_val(entry, _val_) \
    do { entry->v.u64 = _val_; } while(0)

#define dictionary_free_key(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

#define dictionary_set_key(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)

#define dictionary_compare_keys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

#define dictionary_hash_key(d, key) (d)->type->hashFunction(key)
#define dictionary_item_get_key(he) ((he)->key)
#define dictionary_item_get_val(he) ((he)->v.val)
#define dictionary_item_get_signed_integer_val(he) ((he)->v.s64)
#define dictionary_item_get_unsigned_integer_val(he) ((he)->v.u64)
#define dictionary_slots(d) ((d)->ht[0].size+(d)->ht[1].size)
#define dictionary_size(d) ((d)->ht[0].used+(d)->ht[1].used)
#define dictionary_is_rehashing(ht) ((ht)->rehashidx != -1)

/** 创建一个新字典 */
dictionary_t *dictionary_create(dictionary_type_t *type, void *privdata);

/** 对字典进行扩展 */
int dictionary_expand(dictionary_t *dictionary, unsigned long size);

/**
 * 将元素添加到目标哈希表中
 * @param[in] d 字典指针
 * @param[in] key 新元素的关键字
 * @param[in] val 新元素的值
 * @returns 添加成功为 0，添加出错为 -1
 */
int dictionary_add(dictionary_t *dictionary, void *key, void *val);
/**
 * 将元素添加到目标哈希表中
 * 功能与 dictionary_add() 相同，但必须指定 valDup 才能添加成功
*/
int dictionary_add_copy(dictionary_t *dictionary, void *key, const void *val);

/** 添加元素的底层实现函数(由 dictionary_add 调用) */
dictionary_item_t *dictionary_add_raw(dictionary_t *dictionary, void *key);

/**
 * 将新元素添加到字典，如果 key 已经存在，那么新元素覆盖旧元素。
 * @param[in] key 新元素的关键字
 * @param[in] val 新元素的值
 * @return 1 key 不存在，新建元素添加成功
 * @return 0 key 已经存在，旧元素被新元素覆盖
 */
int dictionary_replace(dictionary_t *dictionary, void *key, void *val);
dictionary_item_t *dictionary_replace_raw(dictionary_t *dictionary, void *key);
int dictionary_delete(dictionary_t *dictionary, const void *key);

//从字典中删除包含给定键的节点，但不调用键值的释放函数来删除键值
int dictionary_delete_no_free(dictionary_t *dictionary, const void *key);

/** 删除字典，释放内存资源 */
void dictionary_release(dictionary_t *dictionary);

/**
 * 在字典中按指定的 key 查找
 * 查找过程是典型的 separate chaining find 操作
 * 具体参见：http://en.wikipedia.org/wiki/hash__table#separate_chaining
 */
dictionary_item_t * dictionary_find(dictionary_t *dictionary, const void *key);

/** 查找给定 key 在字典 d 中的值 */
void *dictionary_fetch_value(dictionary_t *dictionary, const void *key);

/** 重新调整字典的大小，缩减多余空间 */
int dictionary_resize(dictionary_t *dictionary);

/**
 * 创建一个迭代器，用于遍历哈希表节点。
 *
 * safe 属性指示迭代器是否安全，如果迭代器是安全的，那么它可以在遍历的过程中
 * 进行增删操作，反之，如果迭代器是不安全的，那么它只能执行 dictionary_next 操作。
 *
 * 因为迭代进行的时候可以对列表的当前节点进行修改，为了避免修改造成指针丢失，
 * 所以不仅要有指向当前节点的 entry 属性，还需要指向下一节点的 next_entry 属性
 */
dictionary_iterator_t *dictionary_get_iterator(dictionary_t *dictionary);

/** 创建一个安全迭代器 */
dictionary_iterator_t *dictionary_get_safe_iterator(dictionary_t *dictionary);

/** 迭代器的推进函数 */
dictionary_item_t *dictionary_next_iterator(dictionary_iterator_t *iter);

/** 删除迭代器 */
void dictionary_release_iterator(dictionary_iterator_t *iter);

/** 从字典中随机获取一项 */
dictionary_item_t *dictionary_get_random_key(dictionary_t *dictionary);

/** 打印字典的统计信息 */
void dictionary_print_stats(dictionary_t *dictionary);

unsigned int dictionary_make_hash(const unsigned char *buf, int len);
unsigned int dictionary_make_case_hash(const unsigned char *buf, int len);
unsigned int dictionary_make_int_hash(unsigned int key);

/* Identity hash function for integer keys */
unsigned int dictionary_identity_hash(unsigned int key);

/** 清空字典 */
void dictionary_empty(dictionary_t *dictionary);
void dictionary_enable_resize(void);
void dictionary_disable_resize(void);

/**
 * 字典的 rehash 函数
 * @param[in] n 要执行 rehash 的元素数量
 * @return 0 所有元素 rehash 完毕
 * @return 1 还有元素没有 rehash
*/
int dictionary_rehash(dictionary_t *dictionary, int num);

/*
 * 在指定的时间内完成 rehash 操作
 * @param[in] ms 进行 rehash 的时间，以毫秒为单位
 * @returns rehashes 完成 rehash 的元素的数量
 */
int dictionary_rehash_milliseconds(dictionary_t *dictionary, int ms);
void dictionary_set_hash_seed(unsigned int initval);
unsigned int dictionary_get_hash_seed(void);

unsigned int string_key_dictionary_get_key_hash(const void *key);
int string_key_dictionary_compare_key(void *privdata, const void *key1,
				      const void *key2);
void *string_key_dictionary_duplicate_key(void *privdata, const void *key);
void string_key_dictionary_destroy_key(void *privdata, void *key);

void dictionary_init_string_key_type(dictionary_type_t *type);
void dictionary_init_string_key_type_copy(dictionary_type_t *type);

#endif /* LCUI_UTIL_dICT_H */
