#ifndef __RUBEST_COMMON__
#define __RUBEST_COMMON__

#include <cassert>

/// Fast mul by 6
inline unsigned char fast_mul6(unsigned char x)
{
	assert(x<6);

	return (((x<<1)+x)<<1);
}

/// Fast mul by 36
inline unsigned char fast_mul36(unsigned char x)
{
	assert(x<6);

	return (x<<5) + (x<<2);
}

#endif // __RUBEST_COMMON__


