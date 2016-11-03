#ifndef _BEZIER_SURFACE_H_
#define _BEZIER_SURFACE_H_

#include "point3d_vec.h"
#include "status.h"

typedef struct
{
	point3d_vec_t *ctrls;
} bezier_surface_t;

bezier_surface_t *bezier_surface_initialize(void);
void bezier_surface_uninitialize(bezier_surface_t *surface);
void bezier_surface_print_to_iv(bezier_surface_t *surface, double radius, FILE *stream);
status_t bezier_surface_calculate_mesh(bezier_surface_t *surface, point3d_vec_t *points, double du, double dv);

#endif
