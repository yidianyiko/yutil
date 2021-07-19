# linkedlist_linux



使用传统的链表结构会造成大量的冗余代码, 降低代码的重用性的问题

```c
//传统链表
struct listNode {
	struct list *pre;
	struct list *next;
	void *data;
};
```

而Linux内核源代码中有很多地方都需要用双向循环链表来组织数据。因此,Linux内核源码在`include/linux/list.h`中提供了双向循环链表。我们可以通过直接修改或者学习该其实现方式。

Linux 中的链表采用了数据与结点分离的实现方式。这样在进行遍历等操作的时候，就可以大大的提升效率。

```c
//双循环链表
struct list_head {
	struct list_head *next, *prev;
};

//用于hash表的链表
/* 链表头 */
struct hlist_head {
	struct hlist_node *first;
};
/* 链表节点 */
struct hlist_node {
	struct hlist_node *next, **pprev;
};

```

在这个结构体类型定义中, 并没有数据成员,其主要作用就是**嵌套在其它结构体类型的定义中起**到链接作用。

## 基础操作

### 初始化

```c
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)
/**
 * INIT_LIST_HEAD - Initialize a list_head structure
 * @list: list_head structure to be initialized.
 *
 * Initializes the list_head to point to itself.  If it is a list header,
 * the result is an empty list.
 */
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	WRITE_ONCE(list->next, list);
	list->prev = list;
}
```

上面的宏完成了链表的初始化，将链表的前后指针指向了自己。

### ……

## 实例

以下的链表构造, 只需要在定义一个list_head指针成员，即可将这种结构体类型的结点链接起来, 形成链表。不管链表中节点的数据结构怎么变化，它的链表头只需要占8个字节。

```c
//链表中的节点则如下定义
struct list {
	struct list_head list_head;
	void *data;
};
```

## 优势：

> Linux的内核链表的一个突出优点是：由于可以方便地将其标准实现（即“小结构体”）镶嵌到任意节点当中，因此任何数据组成的链表的所有操作都被完全统一。另外，即使在代码维护过程中要对节点成员进行升级修改，也完全不影响该节点原有的链表结构。
>
> 内核链表的整体结构分为数据域和指针域两部分。指针域分为头指针和尾指针，头指针指向前一个数据，尾指针指向下一个数据；而头结点则让整个链表产生循环关系。信息存储在堆空间里的存储形式如图3所示。

![图3 数据存储状态示意图](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/5b7160be899b47f39259c3316165e9ff~tplv-k3u1fbpfcp-zoom-1.image)[数据存储状态示意图](https://link.juejin.cn/?target=https%3A%2F%2Fkns.cnki.net%2FKXReader%2FDetail%2FGetImg%3Ffilename%3Dimages%2FELEW202102075_01800.jpg%26uid%3DWEEvREcwSlJHSldTTEYzVTFPU25OeDA5Nnhqck1sYUg4YWpTNFhubDdoTT0%3D%249A4hF_YAuvQ5obgVAqNKPCYcEjKensW4IQMovwHtwkF4VYPoHbKxJw!!)

## 使用场景

1. 在C语言程序中若想根据需要动态地分配和释放内存单元, 通常可以使用链表结构来组织数据；
2. 可以在用户程序自定义的结构体类型中定义一个或多个list_head指针, 用于链接不同的链表。

**参考：**

- [浅析Linux Kernel中的那些链表 - tangr206 - 博客园 (cnblogs.com)](https://www.cnblogs.com/tangr206/articles/3291029.html)
- Linux存储结构分析_毛婷
