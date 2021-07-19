# dict



## LCUI & redis 

### dict

哈希表

实现方式：

```c
/** 哈希表节点结构 */
typedef struct DictEntry {
	void *key;
	union {
		void *val;
		uint64_t u64;
		int64_t s64;
	} v;
	struct DictEntry *next; /**< 指向下一个哈希节点(形成链表) */
} DictEntry;

/** 字典内数据的类型 */
typedef struct DictType {
	unsigned int(*hashFunction)(const void *key);
	void *(*keyDup)(void *privdata, const void *key);
	void *(*valDup)(void *privdata, const void *obj);
	int(*keyCompare)(void *privdata, const void *key1, const void *key2);
	void(*keyDestructor)(void *privdata, void *key);
	void(*valDestructor)(void *privdata, void *obj);
} DictType;

/** 哈希表结构 */
typedef struct DictHashTable {
	DictEntry **table;	/**< 节点指针数组 */
	unsigned long size;	/**< 桶的数量 */
	unsigned long sizemask;	/**< mask 码，用于地址索引计算 */
	unsigned long used;	/**< 已有节点数量 */
} DictHashTable;

/** 字典结构 */
typedef struct Dict {
	DictType *type;		/**< 为哈希表中不同类型的值所使用的一族函数 */
	void *privdata;
	DictHashTable ht[2];	/**< 每个字典使用两个哈希表 */
	int rehashidx;		/**< rehash 进行到的索引位置，如果没有在 rehash ，就为 -1 */
	int iterators;		/**< 当前正在使用的 iterator 的数量 */
} Dict;

/** 用于遍历字典的迭代器 */
typedef struct DictIterator {
	Dict *d;		/**< 迭代器所指向的字典 */
	int table;		/**< 使用的哈希表号码 */
	int index;		/**< 迭代进行的索引 */
	int safe;		/**< 是否安全 */
	DictEntry *entry;	/**< 指向哈希表的当前节点 */
	DictEntry *next_entry;	/**< 指向哈希表的下个节点 */
} DictIterator;
```



**具体使用场景**见`RA/dict.md`

- Dict_Create
- Dict_Expand
- Dict_Add
- Dict_Addcop
- Dict Addraw
- Dict_Replace
- Dict_Replaceraw
- Dict_Deletenofree
- Dict_Release
- Dict_Find
- Dict_Fetchvalue
- Dict_Resize
- Dict_Getlterator
- Dict_Getsafelterator
- Dict_Next
- Dict_Releaselterator
- Dict_Getrandomkey
- Dict_Printstats
- Dict_Genhashfunction
- Dict_Genhashfunction
- Dict _nthashfunction
- Dict_dentityhashfunction
- Dict_Empty
- Dict_Enableresize
- Dict_Disable Resize
- Dict_Rehash
- Dict_Rehashmilliseconds
- Dict_SethashfunctionSeed
- Dict_Gethashfunctionseed
- Stringkeydict_keyhash
- Stringkey Dict_KeyCompare
- Stringkeydict_keydup
- Stringkeydict_keydestructor
- Dict_initstringkeytype
- Dict_initstring CopykeyType

**参考：**

- [Redis内部数据结构详解之字典(dict)_fmt.Println(-CSDN博客](https://blog.csdn.net/acceptedxukai/article/details/17484431)



## Tbox

### *dictionary*

**实现方式：**

```c
/// the dictionary item type
typedef struct __tb_oc_dictionary_item_t
{
    /// the key
    tb_char_t const*    key;

    /// the value
    tb_object_ref_t     val;

}tb_oc_dictionary_item_t;

// the dictionary type
typedef struct __tb_oc_dictionary_t
{
    // the object base
    tb_object_t         base;

    // the capacity size
    tb_size_t           size;

    // the object hash
    tb_hash_map_ref_t   hash;

    // increase refn?
    tb_bool_t           incr;

}tb_oc_dictionary_t;


```



**使用场景**

- 要创建 dictionary，使用 ``tb_oc_dictionary_init() `。
- 返回字典大小，使用`tb_oc_dictionary_size()`
- 设置引用，使用`tb_oc_dictionary_incr()`
- 创建迭代器，使用`tb_oc_dictionary_itor();`
- 插入键和值，使用 ``tb_oc_dictionary_insert()` 。
- 若要查找与给定键对应的值，使用 ``tb_oc_dictionary_value ()`。
- 若要删除键和值，使用 `tb_oc_dictionary_remove () `。

## Glib

### [Hash Tables](https://developer.gnome.org/glib/stable/glib-Hash-Tables.html)

GHashTable 提供键和值之间的关联，这些键和值经过优化后可以在分摊 o (1)中找到、插入或删除关联值。遍历每个元素的所有操作都需要 o (n)时间(列出所有键/值、表调整大小等)。

**实现方式：**

```c
struct _GHashTable
{
  gsize            size;
  gint             mod;
  guint            mask;
  gint             nnodes;
  gint             noccupied;  /* nnodes + tombstones */

  guint            have_big_keys : 1;
  guint            have_big_values : 1;

  gpointer         keys;
  guint           *hashes;
  gpointer         values;

  GHashFunc        hash_func;
  GEqualFunc       key_equal_func;
  gatomicrefcount  ref_count;
#ifndef G_DISABLE_ASSERT
  /*
   * Tracks the structure of the hash table, not its contents: is only
   * incremented when a node is added or removed (is not incremented
   * when the key or data of a node is modified).
   */
  int              version;
#endif
  GDestroyNotify   key_destroy_func;
  GDestroyNotify   value_destroy_func;
};

typedef struct
{
  GHashTable  *hash_table;
  gpointer     dummy1;
  gpointer     dummy2;
  gint         position;
  gboolean     dummy3;
  gint         version;
} RealIter;


typedef struct _GHashTable  GHashTable;

typedef gboolean  (*GHRFunc)  (gpointer  key,
                               gpointer  value,
                               gpointer  user_data);

typedef struct _GHashTableIter GHashTableIter;

struct _GHashTableIter
{
  /*< private >*/
  gpointer      dummy1;
  gpointer      dummy2;
  gpointer      dummy3;
  int           dummy4;
  gboolean      dummy5;
  gpointer      dummy6;
};

```



**使用场景**

- 要创建 GHashTable，使用 ``g_hash_table_new ()` 。
- 若要向 `GHashTable` 中插入键和值，使用 ``g_hash_table_insert ()` 。
- 若要查找与给定键对应的值，使用 ``g_hash_table_lookup ()`和 `g_hash_table_lookup_extended ()`。
- 还可以使用 `g_ hash_table_lookup_extended ()`简单地检查哈希表中是否存在键。
- 若要删除键和值，使用 `g_hash_table_remove () `。
- 要为每个键和值对调用函数，可以使用 `g_hash_table_foreach ()`或使用迭代器迭代哈希表中的键/值对，请参见 GHashTableIter。没有定义哈希表的迭代顺序，您不能依赖于按照插入键/值的相同顺序对它们进行迭代。
- 使用 `ghash_table_destroy ()`来销毁一个 GHashTable。
- 哈希表的一个常见用例是存储关于一组键的信息，而不将任何特定值与每个键关联。GHashTable 优化了这样做的一种方法: 如果您只存储 `key == value` 的键值对，那么 GHashTable 不会分配内存来存储这些值，如果您的集合很大，这可以节省相当多的空间。函数 `g_hash_table_add ()和 ``g_hash_table_contains ()`被设计成这样使用 GHashTable。
- GHashTable 不是设计用在编译时已知的键和值静态初始化的。若要构建静态哈希表，建议使用 gperf 之类的工具。

## 比较

……
