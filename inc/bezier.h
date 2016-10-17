#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "point3d_vec.h"
#include "polyline.h"
#include "status.h"

typedef struct
{
	point3d_vec_t *ctrl;
} bezier_t;

/*
 * bezier_initialize - creates and initializes a Bezier curve
 * @return - a new Bezier curve structure with the given control points
 */
bezier_t *bezier_initialize(void);

/*
 * bezier_uninitialize - uninitializes a Bezier curve and frees all associated memory
 * @param bezier - the Bezier curve to uninitialize
 */
void bezier_uninitialize(bezier_t *bezier);

/*
 * bezier_calculate_polyline - for a Bezier curve, calculates a polyline using the given increment
 * in the evaluation of the points
 * @param bezier - the Bezier curve for which to create the polyline
 * @param poly   - the output polyline
 * @param inc    - increment to use while calculate points on the polyline
 */
status_t bezier_calculate_polyline(bezier_t *bezier, polyline_t *poly, double inc);

/*
 * bezier_from_hermite - calculates the control points for a Bezier curve based on the cubic Hermite
 * defined by the given endpoints and tensions
 * @param bezier - the Bezier curve into which to place the control points
 * @param p0 - the left endpoint of the Hermite
 * @param p3 - the right endpoint of the Hermite
 * @param t0 - the tangent vector at the left endpoint
 * @param t1 - the tangent vector at the right endpoint
 */
status_t bezier_from_hermite(bezier_t *bezier, point3d_t *p0, point3d_t *p3, point3d_t *t0, point3d_t *t1);

/*
 * bezier_print_to_iv - prints the Bezier curve using spheres for its control poitns in OpenInventor
 * format to the given stream
 * @param bezier - the curve to print
 * @param radius - the radius size to use for the spheres
 * @param stream - the stream to which to print
 */
void bezier_print_to_iv(bezier_t *bezier, double radius, FILE *stream);

#endif
