#include <assert.h>
#include <math.h>

#include "transforms.h"

#include "matrix.h"

#define CHECK_SIZE(array)\
	static_assert(sizeof(array) == 16 * sizeof(*array), "Messed up array size.")

#define TRANS_ARRAY(x, y, z)\
	double array[] =\
	{\
		1.0, 0.0, 0.0, x,\
		0.0, 1.0, 0.0, y,\
		0.0, 0.0, 1.0, z,\
		0.0, 0.0, 0.0, 1.0\
	};\
	CHECK_SIZE(array)

matrix_t *translation_matrix(double x, double y, double z)
{
	TRANS_ARRAY(x, y, z);
	return matrix_initialize_with_array(4, 4, array);
}

void translation_matrix_assign(matrix_t *m, double x, double y, double z)
{
	TRANS_ARRAY(x, y, z);
	matrix_assign_from_array(m, array);
}

#define ROTATE_ARRAY_X(t)\
	double array[] =\
	{\
		1, 0, 0, 0,\
		0, cos(t), -sin(t), 0,\
		0, sin(t), cos(t), 0,\
		0, 0, 0, 1\
	};\
	CHECK_SIZE(array)

matrix_t *rotation_matrix_x(double t)
{
	ROTATE_ARRAY_X(t);
	return matrix_initialize_with_array(4, 4, array);
}

void rotation_matrix_x_assign(matrix_t *m, double t)
{
	ROTATE_ARRAY_X(t);
	matrix_assign_from_array(m, array);
}

#define ROTATE_ARRAY_Y(t)\
	double array[] =\
	{\
		cos(t), 0, sin(t), 0,\
		0, 1, 0, 0,\
		-sin(t), 0, cos(t), 0,\
		0, 0, 0, 1\
	};\
	CHECK_SIZE(array)

matrix_t *rotation_matrix_y(double t)
{
	ROTATE_ARRAY_Y(t);
	return matrix_initialize_with_array(4, 4, array);
}

void rotation_matrix_y_assign(matrix_t *m, double t)
{
	ROTATE_ARRAY_Y(t);
	matrix_assign_from_array(m, array);
}

#define ROTATE_ARRAY_Z(t)\
	double array[] =\
	{\
		cos(t), -sin(t), 0, 0,\
		sin(t), cos(t), 0, 0,\
		0, 0, 1, 0,\
		0, 0, 0, 1\
	};\
	CHECK_SIZE(array)

matrix_t *rotation_matrix_z(double t)
{
	ROTATE_ARRAY_Z(t);
	return matrix_initialize_with_array(4, 4, array);
}

void rotation_matrix_z_assign(matrix_t *m, double t)
{
	ROTATE_ARRAY_Z(t);
	matrix_assign_from_array(m, array);
}
