#ifndef _CUBOID_H_
#define _CUBOID_H_

#include "matrix.h"
#include "point3d.h"
#include "status.h"

typedef struct
{
	point3d_t *lowleft;
	point3d_t *upright;
} cuboid_t;

#define CUBOID_POINTS 8

cuboid_t *cuboid_initialize(void);
void cuboid_uninitialize(cuboid_t *cuboid);
void cuboid_set_corners(cuboid_t *cuboid, point3d_t *lowleft, point3d_t *upright);
void cuboid_print_to_iv(cuboid_t *cuboid, FILE *stream);

status_t cuboid_initialize_matrices(matrix_t **matrices, double *ll, double *ur);
void cuboid_uninitialize_matrices(matrix_t **matrices);
void cuboid_print_matrices_to_iv(matrix_t **matrices, FILE *stream);

#endif
