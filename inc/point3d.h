#ifndef _POINT3D_H_
#define _POINT3D_H_

#include <stdio.h>

#include "matrix.h"

typedef struct
{
	double x;
	double y;
	double z;
} point3d_t;

/*
 * point3d_initialize - returns a new instance of a 3D point, with all coordinates set to 0
 * @return - the new 3D point
 */
point3d_t *point3d_initialize(void);

/*
 * point3d_initialize_with_coords - returns a new 3D point, initialized to the given coordinates
 * @param x - the x coordiante
 * @param y - the y coordinate
 * @param z - the z coordinate
 * @return - a new point3d_t with coordinates (x, y, z)
 */
point3d_t *point3d_initialize_with_coords(double x, double y, double z);

/*
 * point3d_copy - copies an existing point and returns a new instance
 * @param orig - the point to copy
 * @return - a new copy of the original point
 */
point3d_t *point3d_copy(point3d_t *orig);

/*
 * point3d_assign - copies the data values from the source point into the destination point
 * @param dst - the point into which to copy the values
 * @param src - the point from which to copy the values
 */
void point3d_assign(point3d_t *dst, point3d_t *src);

/*
 * point3d_uninitialize - uninitializes a 3D point, freeing associated memory
 * @param point - the point to uninitialize
 */
void point3d_uninitialize(point3d_t *point);

/*
 * point3d_scale - scales point by the given scalar
 * @param point - the point to scale
 * @param scalar - the scalar by which to multiply
 */
void point3d_scale(point3d_t *point, double scalar);

/*
 * point3d_add - adds the source point to the destination point
 * @param dst - the destination point
 * @param src - the source point
 */
void point3d_add(point3d_t *dst, point3d_t *src);

/*
 * point3d_sub - subtracts the source point from the destination point
 * @param dst - the destination point
 * @param src - the source point
 */
void point3d_sub(point3d_t *dst, point3d_t *src);

/*
 * point3d_fmad - computes the fused multiply add by multiplying the point by a scalar and then
 * adding it
 * @param dst - the destination point
 * @param src - the source point
 * @param s   - the scalar by which to multiply the source point
 */
void point3d_fmad(point3d_t *dst, point3d_t *src, double s);

/*
 * point3d_print_to_iv - prints the point as a white sphere in the OpenInventor file format
 * @param point - the point to print
 * @param file  - the file to which to print
 * @param r     - the radius for the printed sphere
 */
void point3d_print_to_iv(point3d_t *point, FILE *file, double r);

/*
 * point3d_initialize_matrix - initializes a homogeneous matrix representation of the point
 * @param m   - the matrix pointer to initialize
 * @param pos - three element array of the coordinates
 * @return - indication of success or failure in initialization
 */
status_t point3d_initialize_matrix(matrix_t **m, double *pos);

/*
 * point3d_print_matrix_to_iv - prints the given matrix/vector as a point, i.e., a white sphere,
 * in the OpenInventor file format
 * @param point - the point to print
 * @param file  - the file to which to print
 * @param r     - the radius for the printed sphere
 */
void point3d_print_matrix_to_iv(matrix_t *m, FILE *file, double r);
#endif
