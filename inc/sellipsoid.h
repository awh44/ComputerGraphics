#ifndef _SELLIPSOID_H_
#define _SELLIPSOID_H_

#include "mesh.h"
#include "status.h"

typedef struct
{
	double s1;
	double s2;
	double A;
	double B;
	double C;
} sellipsoid_t;

status_t sellipsoid_calculate_mesh_points(sellipsoid_t *sellipsoid, mesh_t *mesh, size_t num_u, size_t num_v);
status_t sellipsoid_calculate_mesh_normals(sellipsoid_t *sellipsoid, mesh_t *mesh);

#endif
