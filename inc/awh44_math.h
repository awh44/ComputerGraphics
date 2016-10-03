#ifndef _AWH44_MATH_H_
#define _AWH44_MATH_H_

#include <stdint.h>

/*
 * combination - computes the combination (k choose i)
 * @param n - the size of the set in which the choosing is to be done
 * @param k - the number of items to choose
 * @return - the number of ways to choose i items from k, (k choose i)
 */
uint64_t combination(uint32_t n, uint32_t k);

#endif
