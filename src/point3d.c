#include <stdlib.h>

#include "point3d.h"

point3d_t *point3d_initialize(void)
{
	point3d_t *point = calloc(1, sizeof *point);
	if (point == NULL)
	{
		return NULL;
	}

	return point;
}

point3d_t *point3d_initialize_with_coords(double x, double y, double z)
{
	point3d_t *point = malloc(sizeof *point);
	if (point == NULL)
	{
		return NULL;
	}

	point->x = x;
	point->y = y;
	point->z = z;

	return point;
}

void point3d_uninitialize(point3d_t *point)
{
	free(point);
}

void point3d_scale(point3d_t *point, double scalar)
{
	point->x *= scalar;
	point->y *= scalar;
	point->z *= scalar;
}
