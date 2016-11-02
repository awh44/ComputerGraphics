#include <math.h>
#include <stdint.h>

#include "awh44_math.h"

uint64_t combination(uint32_t n, uint32_t k)
{
	if (k == 0 || k >= n)
	{
		return 1;
	}

	uint64_t intermediate = n * combination(n - 1, k - 1);
	return intermediate / k;
}

double bernstein_polynomial(uint32_t n, uint32_t i, double u)
{
	uint64_t combo = combination(n, i);
	double u_to_i = pow(u, i);
	double one_minus_u_to_n_minus_i = pow(1 - u, n - i);
	return combo * u_to_i * one_minus_u_to_n_minus_i;
}

