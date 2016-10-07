#ifndef _POLYLINE_H_
#define _POLYLINE_H_

#include <stdio.h>

#include "point3d.h"
#include "point3d_vec.h"
#include "status.h"

typedef struct
{
	point3d_vec_t *points;
} polyline_t;

/*
 * polyline_initialize - returns a new polyline with no points
 * @return - the new polyline
 */
polyline_t *polyline_initialize(void);

/*
 * polyline_uninitialize - uninitializes a polyline and frees all memory associated with it
 * @param polyline - the polyline to uninitialize
 */
void polyline_uninitialize(polyline_t *poly);

/*
 * polyline_append_point - appends a new point to the end of the polyline
 * @param polyline - the polyline to which the point will be appended
 * @param point    - the point to append (shallowly copied)
 */
void polyline_append_point(polyline_t *poly, point3d_t *point);

/*
 * polyline_append_point - appends a new point to the end of the polyline
 * @param polyline - the polyline to which the point will be appended
 * @param point    - the point to append (deeply copied)
 */
status_t polyline_copy_and_append_point(polyline_t *poly, point3d_t *point);

/*
 * polyline_print - prints the polyline in OpenInventor format to the given stream
 * @param poly   - the polyline to print
 * @param stream - the stream to which to print
 */
void polyline_print_to_iv(polyline_t *poly, FILE *stream);
#endif
