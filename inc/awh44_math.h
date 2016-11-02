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

/*
 * bernstein_polynomial - calculates the value of a Bernstein polynomial at u, i.e.,
 * (n choose i) * u^i * (1 - u)^(n - i)
 */
double bernstein_polynomial(uint32_t n, uint32_t i, double u);

#endif
