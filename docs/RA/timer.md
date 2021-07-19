# 定时器 timer

定时器用于将一些操作推迟到指定时间之后执行。

[LCUI 的定时器](https://docs.lcui.lc-soft.io/app/timer)都是在主线程中处理的，这意味着定时器的时间粒度受到帧率的限制，不能小于每帧的停留时间。

## 函数概要

| 限定符和类型 | 函数和说明                                                   |
| :----------- | :----------------------------------------------------------- |
| int          | `LCUITimer_Set();` <br />设置定时器                          |
| int          | `LCUI_SetTimeout();`<br />设置一个非重复定时器。             |
| int          | `LCUI_SetInterval();`<br />设置定时器重复调用`OnInterval()` 的间隔时间 |
| int          | `LCUITimer_Free();`<br />释放定时器                          |
| int          | `LCUITimer_Pause();`<br />暂停定时器的倒计时                 |
| int          | `LCUITimer_Continue();`<br />继续定时器的倒计时              |
| int          | `LCUITimer_Reset();`<br />重设定时器的等待时间               |
| size_t       | `LCUI_ProcessTimers();`<br />处理所有定时器                  |
| void         | `LCUI_InitTimer();`<br />初始化定时器                        |
| void         | `LCUI_FreeTimer();`<br />释放定时器                          |


## LCUITimer_Set

```c
int LCUITimer_Set(long int n_ms, TimerCallback callback,
			   void *arg, LCUI_BOOL reuse);
```

设置定时器。定时器的作用是让一个任务在经过指定时间后才执行。

**参数说明:**

- `n_ms` – 等待的时间，单位为毫秒
- `callback_` – 用于响应定时器的回调函数
- `args` – 定时器描述
- `reuse` – 指示该定时器是否重复使用，如果要用于循环定时处理某些任 务，可将它置为 TRUE，否则置于 FALSE。

**返回说明：**

- 该定时器的标识符

**依赖它的文件：**

- **src/gui/widget/scrollbar.c**

## LCUI_SetTimeout

```c
/** repeatedly calls a function, with a fixed time delay between each call. */
LCUI_API int LCUI_SetTimeout(long int n_ms, TimerCallback callback, void *arg);

```

设置一个非重复定时器。

**参数说明:**

- 见`LCUITimer_Set`，其中`reuse` –置 FALSE。

**返回说明：**

- 该定时器的标识符

**依赖它的文件：**

- **src/gui/widget/textcaret.c**



## LCUI_SetInterval

```c
int LCUI_SetInterval(long int n_ms, TimerCallback callback,
			      void *arg);

```

设置一个重复定时器。

**参数说明:**

- 见`LCUITimer_Set`，其中的`reuse` 置为 TRUE。

**返回说明：**

- 该定时器的标识符

**依赖它的文件：**

- **src/gui/widget/scrollbar.c**



## LCUITimer_Free

```c
int LCUITimer_Free(int timer_id)
```

释放定时器。当不需要定时器时，可以使用该函数释放定时器占用的资源，并移除程序任务队列中还未处理的定时器任务

**参数说明:**

- `timer_id` – 需要释放的定时器的标识符。

**返回说明：**

-  正常返回0，
- 指定ID的定时器不存在则返回-1.

**依赖它的文件：**

- **src/gui/widget/scrollbar.c**
- **src/gui/widget/textcaret.c**



## LCUITimer_Pause

```c
int LCUITimer_Pause(int timer_id)
```

暂停定时器的倒计时。一般用于往复定时的定时器

**参数说明:**

- `timer_id` – 目标定时器的标识符

**返回说明：**

- 正常返回0，指定ID的定时器不存在则返回-1.

**依赖它的文件：**

- 





## LCUITimer_Continue

```c
int LCUITimer_Continue(int timer_id);
```

继续定时器的倒计时

**参数说明:**

- `timer_id` – 目标定时器的标识符

**返回说明：**

- 正常返回0，指定ID的定时器不存在则返回-1.

**依赖它的文件：**

- 

## LCUITimer_Reset

重设定时器的等待时间

**参数:**

**参数说明:**

- `timer_id` – 需要释放的定时器的标识符
- `n_ms` – 等待的时间，单位为毫秒

**返回说明：**

- 正常返回0，指定ID的定时器不存在则返回-1.

**依赖它的文件：**

- **src/gui/widget/textcaret.c**



## LCUI_ProcessTimers

```c
size_t LCUI_ProcessTimers(void);
```

执行所有定时器

**参数说明:**

- 

**返回说明：**

- 定时器的数量

**依赖它的文件：**

- **src/main.c**

## LCUI_InitTimer

```c
void LCUI_InitTimer(void);
```

初始化定时器

**参数说明:**

- 

**返回说明：**

- 

**依赖它的文件：**

- **src/main.c**

## LCUI_FreeTimer

```c
void LCUI_FreeTimer(void);
```

释放定时器

**参数说明:**

- 

**返回说明：**

- 

**依赖它的文件：**

- **src/main.c**



**存在问题：**

1. 全局共用同一个定时器列表
2. 用到了线程操作、互斥锁、链表、时间相关的函数，依赖项较多，与 util 的依赖少特性不符。

**改进方案：**

- **重新设计定时器的接口**

  改用面向对象模式代替单例模式，示例：

  

  ```
  // 当前的接口设计LCUI_InitTimer();LCUI_FreeTimer();LCUITimer_Set();LCUITimer_Free();LCUI_ProcessTimers();
  // 新的接口设计TimerList_New();TimerList_Free();TimerList_Add();TimerList_Remove();TimerList_Process();
  ```

  在新的设计中，不再是全局共用同一个定时器列表，允许开发者基于定时器接口实现一个定时器线程，以此摆脱 LCUI 主循环对定时器精度的影响。

- 重新设计后的 timer 只需要依赖链表和时间相关函数
