#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef round
#define round(x) ((x) > 0 ? (int)((x) + 0.5) : (int)((x)-0.5))
#endif

#define pow2(x) ((x) * (x))

#endif /* UTIL_MATH_H */