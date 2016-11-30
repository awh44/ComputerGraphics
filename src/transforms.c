#include <assert.h>
#include <math.h>

#include "transforms.h"

#include "matrix.h"

matrix_t *translation_matrix(double x, double y, double z)
{
	double array[] =
	{
	  1.0, 0.0, 0.0, x,
	  0.0, 1.0, 0.0, y,
	  0.0, 0.0, 1.0, z,
	  0.0, 0.0, 0.0, 1.0
	};
	static_assert(sizeof(array) == 16 * sizeof(*array), "Messed up array size.");

	return matrix_initialize_with_array(4, 4, array);
}

matrix_t *rotation_matrix_x(double t)
{
	double array[] =
	{
		1, 0, 0, 0,
		0, cos(t), -sin(t), 0,
		0, sin(t), cos(t), 0,
		0, 0, 0, 1
	};
	static_assert(sizeof(array) == 16 * sizeof(*array), "Messed up array size.");

	return matrix_initialize_with_array(4, 4, array);
}

matrix_t *rotation_matrix_y(double t)
{
	double array[] =
	{
		cos(t), 0, sin(t), 0,
		0, 1, 0, 0,
		-sin(t), 0, cos(t), 0,
		0, 0, 0, 1
	};
	static_assert(sizeof(array) == 16 * sizeof(*array), "Messed up array size.");

	return matrix_initialize_with_array(4, 4, array);
}

matrix_t *rotation_matrix_z(double t)
{
	double array[] =
	{
		cos(t), -sin(t), 0, 0,
		sin(t), cos(t), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	static_assert(sizeof(array) == 16 * sizeof(*array), "Messed up array size.");

	return matrix_initialize_with_array(4, 4, array);
}
