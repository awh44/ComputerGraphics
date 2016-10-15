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
 *                             in the evaluation of the points
 * @param bezier - the Bezier curve for which to create the polyline
 * @param poly   - the output polyline
 * @param inc    - increment to use while calculate points on the polyline
 */
status_t bezier_calculate_polyline(bezier_t *bezier, polyline_t *poly, double inc);

/*
 * bezier_print - prints the Bezier curve using spheres for its control poitns in OpenInventor
 *                format to the given stream
 * @param bezier - the curve to print
 * @param radius - the radius size to use for the spheres
 * @param stream - the stream to which to print
 */
void bezier_print_to_iv(bezier_t *bezier, double radius, FILE *stream);

#endif
