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
