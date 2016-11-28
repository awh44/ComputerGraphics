#ifndef _CUBOID_H_
#define _CUBOID_H_

#include "point3d.h"

typedef struct
{
	point3d_t *lowleft;
	point3d_t *upright;
} cuboid_t;

cuboid_t *cuboid_initialize(void);
void cuboid_uninitialize(cuboid_t *cuboid);
void cuboid_set_corners(cuboid_t *cuboid, point3d_t *lowleft, point3d_t *upright);
void cuboid_print_to_iv(cuboid_t *cuboid, FILE *stream);

#endif
