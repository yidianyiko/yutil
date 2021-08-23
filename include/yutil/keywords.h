#ifndef UTIL_KEYWORDS_H
#define UTIL_KEYWORDS_H

// keyword
#if defined(_WIN32) && !defined(__cplusplus)
#define _y_inline __inline
#else
#define __y_inline inline
#endif

#ifdef __GNUC__
#define Y_API extern
#else

#ifdef Y_EXPORTS
#define Y_API __declspec(dllexport)
#else
#define Y_API __declspec(dllimport)
#endif

#endif
// extern c
#ifdef __cplusplus

#define Y_BEGIN_HEADER extern "C" {
#define Y_END_HEADER }
#else

#define Y_BEGIN_HEADER
#define Y_END_HEADER
#endif

#endif    // UTIL_KEYWORDS_H