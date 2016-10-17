#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdint.h>
#include <stdio.h>

#include "point3d.h"
#include "point3d_vec.h"
#include "status.h"

/*
 * read_points - read in a series of points from a file in the format:
 * x0 y0 z0
 * x1 y1 z1
 * ...
 * xn yn zn
 * Note that on error, the function will not free the points it has already placed in the vector
 * @param stream - the file stream from which to read the points
 * @param points - the vector into which to place the points
 * @return       - an indication of whether an error occured
 */
status_t read_points(FILE *stream, point3d_vec_t *points);

/*
 * parse_point - given a line from a file containing point data, reads it into the point3d_t
 * structure
 * @param stream - the file stream from which to read the point
 * @param point  - the point of which to fill the coordinates
 */
status_t parse_point(char *line, point3d_t *point);

#endif
