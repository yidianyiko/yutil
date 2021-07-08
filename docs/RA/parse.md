# parse

从字符串中解析数据。

## 函数概要

| 限定符和类型 | 函数和说明                                                   |
| :----------- | :----------------------------------------------------------- |
| LCUI_BOOL    | `ParseNumber` <br />从字符串中解析出数字，支持的单位：点(pt)、像素(px)、百分比(%) |
| LCUI_BOOL    | `ParseRGB`<br />从字符串中解析出RBG值                        |
| LCUI_BOOL    | `ParseRGBA`<br />从字符串中解析出RBGA值                      |
| LCUI_BOOL    | `ParseColor`<br />从字符串中解析出色彩值                     |
| LCUI_BOOL    | `ParseUrl` <br />从字符串中解析资源路径                      |
| LCUI_BOOL    | `ParseFontWeight`<br />从字符串中解析字磅                    |
| LCUI_BOOL    | `ParseFontStyle`<br />从字符串中解析字体风格                 |



## ParseNumber 

```c
LCUI_BOOL ParseNumber(LCUI_Style var, const char *str);

```

从字符串中解析出数字，支持的单位：点(pt)、像素(px)、百分比(%) 

**参数说明：**

- `var`：目标（LCUI_Style型）缓存，存放解析数据
- `str`：目标字符串

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 无

**改进方案：**

- 无

**依赖它的文件：**

- src\gui\css_parser.c
- src\gui\css_fontstyle.c
- src\font\textstyle.c

## ParseRGB

```c
LCUI_BOOL ParseRGB(LCUI_Style var, const char *str)
```

从字符串中解析出RBG值

**参数说明：**

- `var`：目标缓存，存放解析数据
- `str`：目标字符串

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- 无

## ParseRGBA

```c
LCUI_BOOL ParseRGBA(LCUI_Style var, const char *str)
```

从字符串中解析出RBGA值

**参数说明：**

- `var`：目标缓存，存放解析数据
- `str`：目标字符串

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- 无

## ParseColor

```c
LCUI_BOOL ParseColor(LCUI_Style var, const char *str);
```

从字符串中解析出色彩值，支持格式：#fff、#ffffff, rgba(R,G,B,A)、rgb(R,G,B) 

**参数说明：**

- `var`：目标缓存，存放解析数据
- `str`：目标字符串

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- src\gui\css_parser.c
- src\gui\css_fontstyle.c
- src\font\textstyle.c

## ParseUrl

```c
LCUI_BOOL ParseUrl(LCUI_Style s, const char *str, const char *dirname)
```

从字符串中解析资源路径 

**参数说明：**

- `s`：目标缓存，存放解析数据
- `str`：目标字符串
- `dirname`：目标缓存，存放解析数据

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- src\gui\css_rule_font_face.c
- src\gui\css_parser.c

## ParseFontWeight

```c
LCUI_BOOL ParseFontWeight(const char *str, int *weight)

```

从字符串中解析字磅

**参数说明：**

- `str`：目标字符串
- `weight`：目标缓存，存放解析数据

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- src\gui\css_fontstyle.c
- src\gui\css_rule_font_face.c

## ParseFontStyle

```c
LCUI_BOOL ParseColor(LCUI_Style var, const char *str);
```

从字符串中解析字体风格

**参数说明：**

- `str`：目标字符串
- `style`：目标缓存，存放解析数据

**返回说明：**

- 成功返回（LCUI_BOOL TRUE）1，失败返回（LCUI_BOOL FALSE）0

**存在问题：**

1. 命名风格

**改进方案：**

- 修改命名风格。

**依赖它的文件：**

- src\gui\css_rule_font_face.c
- src\gui\css_fontstyle.c
