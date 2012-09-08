#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <algorithm>

/* surprisingly useful */
template <class T>
inline T clamp(const T &value, const T &low, const T &high)
{
	return std::max(low, std::min(high, value));
}

/* yuck, #define */
#define FLOAT_EPSILON 1.0e-4f

#endif