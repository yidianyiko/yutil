# rect

提供矩形区域处理函数。

## 函数概要

| 限定符和类型 | 函数和说明                                                   |
| :----------- | :----------------------------------------------------------- |
|              | `LCUIRect_IsIncludeRect` <br />判断矩形b是否包含矩阵a        |
| LCUI_Rect    | `Rect` <br />**将数值转换成LCUI_Rect型结构体**               |
| void         | `LCUIRect_GetCutArea` <br />据容器尺寸，获取指定区域中需要裁剪的区域。 |
|              | `LCUIRect_HasPoint` <br />判断坐标(x,y)是否在矩形类。        |
| LCUI_BOOL    | `LCUIRect_ValidateArea` <br />将矩形区域范围调整在容器有效范围内 |
| LCUI_BOOL    | `LCUIRectF_ValidateArea` <br />将矩形区域范围调整在浮点类型容器有效范围内 |
| void         | `LCUIRect_ToRectF` <br />整形矩形转换为浮点类型的矩形        |
| void         | `LCUIRect_Scale` <br />将double型数据的矩形 四舍五入 转换为整形矩形 |
| void         | `LCUIRectF_ToRect` <br />浮点类型的矩形转换为整形矩形        |
| LCUI_BOOL    | `LCUIRect_IsCoverRect` <br />检测矩形是否遮盖另一个矩形      |
| LCUI_BOOL    | `LCUIRect_GetOverlayRect` <br />获取两个矩形中的重叠矩形     |
| LCUI_BOOL    | `LCUIRectF_GetOverlayRect` <br />获取两个矩形中的重叠矩形（浮点类型） |
| void         | `LCUIRect_MergeRect` <br />合并两个矩形                      |
| void         | `LCUIRectF_MergeRect` <br />检测矩形是否遮盖另一个矩形（浮点类型） |
| void         | `LCUIRect_CutFourRect` <br /> 根据重叠矩形 rect1，将矩形 rect2 分割成四个矩形（rect1被rect2完全包含）。 |
| void         | `LCUIRect_Split` <br /> 根据重叠矩形 rect1，将矩形 rect2 分割成四个矩形（rect1不被rect2完全包含） |
| LCUI_BOOL    | `LCUIRectF_IsEquals` <br /> 判断两个（浮点型）矩形是否相等   |
| LCUI_BOOL    | `LCUIRect_IsEquals` <br /> 判断两个矩形是否相等              |
| int          | `RectList_AddEx` <br />矩阵链表的添加操作                    |
| int          | `RectList_Add` <br />给矩阵链表添加一个脏矩形记录，默认自动合并 |
| int          | `RectList_Delete` <br />矩阵链表的删除操作，删除脏矩形       |



## Rect

```c
  LCUI_Rect Rect(int x, int y, int w, int h);
```

*将数值转换成LCUI_Rect型结构体*

**参数说明：**

- `x`：横坐标
- `y`：纵坐标
- `w`：宽度
- `h`：高度

**返回说明：**

- 返回LCUI_Rect

**存在问题：**

1. 命名风格

**改进方案：**

- 无

**依赖它的文件：**

- 



## LCUIRect_IsIncludeRect

```c
LCUIRect_IsIncludeRect(a, b)
```

判断矩形b是否包含矩阵a

**参数说明：**

- `a`：矩阵a
- `b`：矩阵b

**返回说明：**

- 成功返回 1，不包含返回0

**存在问题：**

1. 命名风格

**改进方案：**

- 修正

**依赖它的文件：**

- src\gui\widget_base.c

## LCUIRect_GetCutArea

```c

  void LCUIRect_GetCutArea(int box_w, int box_h, LCUI_Rect rect,
				  LCUI_Rect *cut);

```

据容器尺寸，获取指定区域中需要裁剪的区域。

**参数说明：**

- `box_w`：容器宽度
- `box_h`：容器高度
- `rect`：目标指定区域
- `cut`：需要裁剪的区域

**返回说明：**

- 无

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- src\font\fontlibrary.c
- src\graph.c

## LCUIRect_HasPoint

```c
#define LCUIRect_HasPoint(rect, X, Y)
```

判断坐标(x,y)是否在矩形类。

**参数说明：**

- `x`：横坐标
- `y`：纵坐标
- `rect`：目标指定区域

**返回说明：**

- 包含返回1，否则返回0

**存在问题：无考虑与LCUI_ReadDirA是否可以合并

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\widget_event.c
- src\gui\widget_tree.c

## LCUIRect_ValidateArea

```c
LCUI_BOOL LCUIRect_ValidateArea(LCUI_Rect *rect, int box_w, int box_h);

```

将矩形区域范围调整在容器有效范围内

**参数说明：**

- `box_w`：容器横坐标
- `box_h`：容器纵坐标
- `rect`：目标矩形

**返回说明：**

- 成功返回（LCUI_BOOL） 1，失败返回（LCUI_BOOL）0；

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\platform\windows\uwp_renderer.cpp
- src\platform\linux\linux_x11display.c
- src\platform\linux\linux_fbdisplay.c
- src\graph.c
- src\font\textlayer.c

## LCUIRectF_ValidateArea

```c
  LCUI_BOOL LCUIRectF_ValidateArea(LCUI_RectF *rect, float box_w,
```

将矩形区域范围调整在浮点类型容器有效范围内

**参数说明：**

- `box_w`：（float类型）容器横坐标
- `box_h`：（float类型）容器纵坐标
- `rect`：目标矩形

**返回说明：**

- 成功返回指向目录下文件名，失败返回 NULL

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\widget_task.c
- src\gui\widget_paint.c
- src\gui\widget_base.c

## LCUIRect_ToRectF

```c
void LCUIRect_ToRectF(const LCUI_Rect *rect, LCUI_RectF *rectf, float scale)
```

整形矩形转换为浮点类型的矩形

**参数说明：**

- `rectf`：存放结果
- `scale`：浮点类型的精度范围
- `rect`：被转换的矩形

**返回说明：**

- 无

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\widget\textview.c
- src\gui\widget\textedit.c
- src\gui\widget_paint.c
- src\display.c

## LCUIRect_Scale

```
void LCUIRect_Scale(const LCUI_Rect *src, LCUI_Rect *dst, float scale);
```

将double型数据的矩形 四舍五入 转换为整形矩形

**参数说明：**

- `dst`：存放转换结果
- `scale`：浮点类型的精度范围
- `src`：被转换的矩形

**返回说明：**

- 成功1，失败返回0；

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- 无

## LCUIRectF_ToRect

```c
void LCUIRectF_ToRect(const LCUI_RectF *rectf, LCUI_Rect *rect, float scale);
```

浮点类型的矩形转换为整形矩形

**参数说明：**

- `rectf`：浮点类型的矩形
- `scale`：浮点类型的精度范围
- `rect`：整形矩形

**返回说明：**

- 无

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- 无



## LCUIRect_IsCoverRect

```c
LCUI_BOOL LCUIRect_IsCoverRect(const LCUI_Rect *a, const LCUI_Rect *b);
```

检测矩形是否遮盖另一个矩形

**参数说明：**

- `b`：矩形b
- `a`：矩形a

**返回说明：**

- 成功返回（LCUI_BOOL） 1，失败返回（LCUI_BOOL）0；

**存在问题：**

1. 是否和`LCUIRect_IsIncludeRect`函数重复？

**改进方案：**

- 调研是够可以代替

**依赖它的文件：**

- 无

## LCUIRect_IsCoverRect

```c
  LCUI_BOOL LCUIRectF_IsCoverRect(const LCUI_RectF *a,
					 const LCUI_RectF *b);
```

检测矩形是否遮盖另一个矩形（浮点类型）

**参数说明：**

- `b`：矩形b
- `a`：矩形a

**返回说明：**

- 成功返回（LCUI_BOOL） 1，失败返回（LCUI_BOOL）0；

**存在问题：**

1. 是否和`LCUIRect_IsIncludeRect`函数重复？

**改进方案：**

- 调研是够可以代替

**依赖它的文件：**

- 无



## LCUIRect_GetOverlayRect

```c
LCUI_BOOL LCUIRect_GetOverlayRect(const LCUI_Rect *a,
					   const LCUI_Rect *b, LCUI_Rect *out);
```

获取两个矩形中的重叠矩形

**参数说明：**

- `a` – 矩形A
- `b` – 矩形B
- `out` – 矩形A和B重叠处的矩形

**返回说明：**

-  如果两个矩形重叠，则返回TRUE，否则返回FALSE

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\widget_paint.c

## LCUIRectF_GetOverlayRect

```c
LCUI_BOOL LCUIRectF_GetOverlayRect(const LCUI_RectF *a, const LCUI_RectF *b, LCUI_RectF *out)；
```

获取两个（浮点类型）矩形中的重叠矩形

**参数说明：**

- `a` – 矩形A
- `b` – 矩形B
- `out` – 矩形A和B重叠处的矩形

**返回说明：**

-  如果两个矩形重叠，则返回TRUE，否则返回FALSE

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\widget_task.c
- src\gui\widget_paint.c

## LCUIRect_MergeRect

```c
void LCUIRect_MergeRect(LCUI_Rect *big, const LCUI_Rect *a,
				 const LCUI_Rect *b);

```

合并两个矩形

**参数说明：**

- `a` – 矩形A
- `b` – 矩形B
- `big` – 矩形A和B合并的矩形

**返回说明：**

- 无

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\platform\windows\uwp_renderer.cpp

## LCUIRectF_MergeRect

```c
void LCUIRectF_MergeRect(LCUI_RectF *big, const LCUI_RectF *a,
				  const LCUI_RectF *b);
```

合并两个浮点类型的矩形

**参数说明：**

- `a` – 矩形A
- `b` – 矩形B
- `big` – 矩形A和B合并的矩形

**返回说明：**

- 无

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\widget_paint.c

## LCUIRect_CutFourRect

```c
LCUI_API void LCUIRect_CutFourRect(LCUI_Rect *rect1, LCUI_Rect *rect2,
				   LCUI_Rect rects[4]);

```

 根据重叠矩形 rect1，将矩形 rect2 分割成四个矩形（rect1被rect2完全包含）

**参数说明：**

- `rect1` – 矩形1
- `rect2` – 矩形2
- ` rects[4]` – 用于存放得到的四个矩形

**返回说明：**

- 无

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- 无

## LCUIRect_Split

```c
void LCUIRect_Split(LCUI_Rect *base, LCUI_Rect *target,
			     LCUI_Rect rects[4]);
```

 根据重叠矩形 rect1，将矩形 rect2 分割成四个矩形（rect1不被rect2完全包含）

**参数说明：**

- `rect1` – 矩形1
- `rect2` – 矩形2
- ` rects[4]` – 用于存放得到的四个矩形

**返回说明：**

- 无

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- 无



## LCUIRectF_IsEquals

```c
INLINE LCUI_BOOL LCUIRectF_IsEquals(const LCUI_RectF *a, const LCUI_RectF *b)
```

 判断两个（浮点型）矩形是否相等

**参数说明：**

- `a` – 矩形A
- `b` – 矩形B

**返回说明：**

- 如果两个矩形相等，则返回TRUE，否则返回FALSE

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- test\libtest.c
- src\gui\widget_diff.c

## LCUIRect_IsEquals

```c
INLINE LCUI_BOOL LCUIRect_IsEquals(const LCUI_Rect *a, const LCUI_Rect *b)
```

 判断两个矩形是否相等

**参数说明：**

- `a` – 矩形A
- `b` – 矩形B

**返回说明：**

- 如果两个矩形相等，则返回TRUE，否则返回FALSE

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- test\libtest.c

## RectList_AddEx

```c
int RectList_AddEx(LinkedList *list, LCUI_Rect *rect,
			    LCUI_BOOL auto_merge);
```

矩阵链表的添加操作

**参数说明：**

- `list` – 链表
- `rect` – 添加的矩形
- `auto_merge` – 是否自动合并

**返回说明：**

- 添加成功，则返回0，*如果被现有的矩形包含*返回-2，其他报错-1.

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\display.c

## RectList_Add

```c
int RectList_Add(LinkedList *list, LCUI_Rect *rect);

```

给矩阵链表添加一个脏矩形记录，默认自动合并

**参数说明：**

- `list` – 链表
- `rect` – 添加的矩形

**返回说明：**

- 添加成功，则返回0，*如果被现有的矩形包含*返回-2，其他报错-1.

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\display.c
- src\platform\linux\linux_fbdisplay.c
- src\font\textlayer.c
- src\draw\boxshadow.c

## RectList_Delete

```
int RectList_Delete(LinkedList *list, LCUI_Rect *rect);
```

矩阵链表的删除操作，删除脏矩形

**参数说明：**

- `list` – 链表
- `rect` – 添加的矩形

**返回说明：**

- 添加成功，则返回1，*如果被现有的矩形包含*返回-2，其他报错-1.

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\draw\boxshadow.c
