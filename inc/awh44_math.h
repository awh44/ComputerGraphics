#ifndef _AWH44_MATH_H_
#define _AWH44_MATH_H_

#include <math.h>
#include <stdint.h>

/*
 * TO_RAD - macro that converts to radians from degrees
 * @param deg - the degrees value to convert
 * @return - the equivalent value in radians
 */
#define TO_RAD(deg) ((M_PI * (deg)) / 180.0)

/*
 * TO_DEG - macro that converts to radians from degrees
 * @param rad- the radians value to convert
 * @return - the equivalent value in degrees
 */
#define TO_DEG(rad) ((180.0 * (rad)) / M_PI)

/*
 * sgn - returns indiciation for sign of given number (-1, 0, or 1)
 * @param n - the number of which to find the sign
 * @return - -1 for negative number, 0 for 0, and 1 for positive number
 */
double sgn(double n);

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
