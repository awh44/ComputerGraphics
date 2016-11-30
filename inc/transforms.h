#ifndef _TRANSFORMS_H_
#define _TRANSFORMS_H_

#include "matrix.h"

/*
 * translation_matrix - returns the 3D, homogeneous translation matrix for the
 * given x, y, and z values
 * @param x - the x-direction translation
 * @param y - the y-direction translation
 * @param z - the z-direction translation
 * @return - the appropriate 4x4 translation matrix for the given x, y, and z
 */
matrix_t *translation_matrix(double x, double y, double z);

/*
 * rotation_matrix_x - returns the 3D, homogeneous rotation matrix around the
 * x-axis for the given angle
 * @param t - the angle to rotate around the x-axis, in radians
 * @return - the appopriate rotation matrix for the rotation angle
 */
matrix_t *rotation_matrix_x(double t); 

/*
 * rotation_matrix_y - returns the 3D, homogeneous rotation matrix around the
 * y-axis for the given angle
 * @param t - the angle to rotate around the y-axis, in radians
 * @return - the appopriate rotation matrix for the rotation angle
 */
matrix_t *rotation_matrix_y(double t);

/*
 * rotation_matrix_z - returns the 3D, homogeneous rotation matrix around the
 * z-axis for the given angle
 * @param t - the angle to rotate around the z-axis, in radians
 * @return - the appopriate rotation matrix for the rotation angle
 */
matrix_t *rotation_matrix_z(double t);

#endif
