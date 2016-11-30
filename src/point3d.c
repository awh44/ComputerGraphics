#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "point3d.h"

#include "matrix.h"
#include "status.h"

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

point3d_t *point3d_copy(point3d_t *orig)
{
	point3d_t *copy = malloc(sizeof *copy);
	if (copy == NULL)
	{
		return NULL;
	}

	point3d_assign(copy, orig);

	return copy;
}

void point3d_assign(point3d_t *dst, point3d_t *src)
{
	memcpy(dst, src, sizeof *dst);
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

void point3d_add(point3d_t *dst, point3d_t *src)
{
	dst->x += src->x;
	dst->y += src->y;
	dst->z += src->z;
}

void point3d_sub(point3d_t *dst, point3d_t *src)
{
	dst->x -= src->x;
	dst->y -= src->y;
	dst->z -= src->z;
}

void point3d_fmad(point3d_t *dst, point3d_t *src, double s)
{
	dst->x += src->x * s;
	dst->y += src->y * s;
	dst->z += src->z * s;
}

void point3d_print_to_iv(point3d_t *point, FILE *file, double r)
{
	fprintf(file,
"Separator {\n\
	LightModel {\n\
		model PHONG\n\
	}\n\
	Material {\n\
		diffuseColor 1.0 1.0 1.0\n\
	}\n\
	Transform {\n\
		translation %lf %lf %lf\n\
	}\n\
	Sphere {\n\
		radius %lf\n\
	}\n\
}\n", point->x, point->y, point->z, r);
}

status_t point3d_initialize_matrix(matrix_t **m, double *pos)
{
	status_t error = SUCCESS;
	double homoarray[] = { pos[0], pos[1], pos[2], 1.0 };
	INITIALIZE_OR_OUT_OF_MEM(*m, matrix_initialize_with_array(4, 1, homoarray), error, error0);
	goto success;
error0:
success:
	return error;
}


void point3d_print_matrix_to_iv(matrix_t *m, FILE *file, double r)
{
	point3d_t p = { matrix_get(m, 0, 0), matrix_get(m, 1, 0), matrix_get(m, 2, 0) };
	point3d_print_to_iv(&p, file, r);
}
