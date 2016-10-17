#ifndef _CATMULLROM_H_
#define _CATMULLROM_H_

#include "point3d_vec.h"
#include "polyline.h"
#include "status.h"

typedef struct
{
	point3d_vec_t *ctrl;
	point3d_t *t0;
	point3d_t *tN;
} catmullrom_t;

/*
 * catmullrom_initialize - initializes a new Catmull-Rom spline and returns it
 * @return - the new Catmull-Rom spline object
 */
catmullrom_t *catmullrom_initialize(void);

/*
 * catmullrom_uninitialize - uninitializes a Catmull-Rom spline
 * @param catmullrom - the spline to uninitialize
 */
void catmullrom_uninitialize(catmullrom_t *catmullrom);

/*
 * catmullrom_calculate_polyline - for a Catmull-Rom spline, calculates a polyline using the given
 * increments
 * @param catmullrom - the spline for which to calculate the polyline
 * @param poly       - the polyline to fill with points
 * @param inc        - the increment to be used when calculating points on the polyline
 */
status_t catmullrom_calculate_polyline(catmullrom_t *catmullrom, polyline_t *poly, double inc);

/*
 * catmullrom_print_to_iv - prints the Catmull-Rom spline using spheres for its control points in
 * OpenInventor format to the given stream
 * @param catmullrom - the spline to print
 * @param radius     - the radius size to use for the control points
 * @param stream     - the stream to which to print
 */
void catmullrom_print_to_iv(catmullrom_t *catmullrom, double radius, FILE *stream);

#endif
