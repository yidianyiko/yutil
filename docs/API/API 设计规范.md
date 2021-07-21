# API 设计规范文档

## **命名规则**

采用小写+下划线命名风格，不使用库名作为接口的前缀名。

### **数据类型**

数据类型名称以 `_t` 结尾，示例：

```
struct foo_t {
    int bar;
};
```

如果是结构体类型，则应该使用 `typedef` 为该类型定义别名：

```
typedef struct foo_t foo_t;
```

不使用 `typedef` 为数据类型的指针定义别名，例如以下写法是不推荐的。

```
typedef struct foo* foo_t;
```

我们推荐将复杂数据结构定义在源文件中，在头文件中只公开由 `typedef` 定义的别名，操作接口只接受该数据结构的指针，示例：

```
// example.h

typedef struct foo_t foo_t;

foo_t *foo_create(int bar);

void foo_destroy(foo_t *foo);

// example.c

#include "example.h"

struct foo_t {
    int bar;
};
```

### **函数命名：**

推荐采用面向对象命名风格，以对象类型名称为前缀，后面接动词或动宾词组，相当于将 `object.doSomething()` 写成 `object_do_something()`。
函数命名中所使用的动词必须是常见的单词，例如：

- get/set
- add/remove
- new/free
- create/destroy
- init/shutdown
- open/close
- start/stop
- begin/end

示例：

```
const char *value;
foo_t *foo;

foo_create(&foo);
foo_set_property(foo, "key", "value");
foo_get_property(foo, "key", &value);
foo_destroy(foo);
```

## 注释

对每个接口，都编写注释，注释规范使用doxygen格式。

参考内容：

- libgit2: https://libgit2.org/libgit2/
- LevelDB: https://github.com/google/leveldb/blob/master/include/leveldb/c.h
- [聊聊tbox的代码风格](https://tboox.org/cn/2016/07/29/code-style/)
- [LCUI 3.0 development plan](https://github.com/lc-soft/LCUI/issues/239)
