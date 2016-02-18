#ifndef UTIL_H
#define UTIL_H

#include <qstring.h>

QString MakePath(const QString &dir, const QString &file);


template <typename T>
inline bool clamp(T& a, const T& min, const T& max)
{
	bool ret = false;
	if (a < min)
	{
		a = min;
		ret = true;
	}
	else if (a > max)
	{
		a = max;
		ret = true;
	}
	return ret;
}

template <typename T>
inline T bound(const T& a, const T& min, const T& max)
{
	if (a < min)
		return min;
	if (a > max)
		return max;
	return a;
}

template<typename V>
inline V mod(const V& a, const V& b)
{
	return (a % b + b) % b;
}
#endif