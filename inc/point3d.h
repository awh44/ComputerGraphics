#ifndef _POINT3D_H_
#define _POINT3D_H_

typedef struct
{
	double x;
	double y;
	double z;
} point3d_t;

point3d_t *point3d_initialize(void);
point3d_t *point3d_initialize_with_coords(double x, double y, double z);
void point3d_uninitialize(point3d_t *point);
void point3d_scale(point3d_t *point, double scalar);


#endif
