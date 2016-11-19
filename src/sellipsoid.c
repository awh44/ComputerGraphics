#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "sellipsoid.h"

#include "awh44_math.h"
#include "mesh.h"
#include "point3d_vec.h"
#include "status.h"

#define POINT_INIT_OR_GOTO(point, errvar, label)\
	do\
	{\
		if ((point = point3d_initialize()) == NULL)\
		{\
			errvar = OUT_OF_MEM;\
			goto label;\
		}\
	} while (0)

#define POINT_INIT_COORDS_OR_GOTO(point, x, y, z, errvar, label)\
	do\
	{\
		if ((point = point3d_initialize_with_coords((x), (y), (z))) == NULL)\
		{\
			errvar = OUT_OF_MEM;\
			goto label;\
		}\
	} while (0)

#define S_EXTRACT(var) double var = sellipsoid->var

#define IF_ERROR_GOTO(expr, errvar, label)\
	do\
	{\
		if ((errvar = (expr)))\
		{\
			goto label;\
		}\
	} while (0)

#ifndef V_INIT
	#define V_INIT (M_PI / 2)
#endif

static double c(double w, double m)
{
	return sgn(cos(w)) * pow(fabs(cos(w)), m);
}

static double s(double w, double m)
{
	return sgn(sin(w)) * pow(fabs(sin(w)), m);
}

static double inline calc_du(size_t num_u)
{
	return (2.0 * M_PI) / (((double) num_u) - 1.0);
}

static double inline calc_dv(size_t num_v)
{
	return -sgn(V_INIT) * fabs(-V_INIT - V_INIT) / (((double) num_v) - 1.0);
}

static status_t add_mesh_point(point3d_vec_t *points, double s1, double s2, double A, double B, double C, double u, double v)
{
	status_t error = SUCCESS;

	point3d_t *new_point;
	POINT_INIT_OR_GOTO(new_point, error, exit0);
	new_point->x = A * c(v, s1) * c(u, s2);
	new_point->y = B * c(v, s1) * s(u, s2);
	new_point->z = C * s(v, s1);

	point3d_vec_push_back(points, new_point);

exit0:
	return error;
}

static status_t add_mesh_normal(point3d_vec_t *normals, double s1, double s2, double A, double B, double C, double u, double v)
{
	status_t error = SUCCESS;

	point3d_t *normal;
	POINT_INIT_OR_GOTO(normal, error, exit0);
	normal->x = (1.0 / A) * c(v, 2 - s1) * c(u, 2 - s2);
	normal->y = (1.0 / B) * c(v, 2 - s1) * s(u, 2 - s2);
	normal->z = (1.0 / C) * s(v, 2 - s1);

	point3d_vec_push_back(normals, normal);

exit0:
	return error;
}

status_t sellipsoid_calculate_mesh_points(sellipsoid_t *sellipsoid, mesh_t *mesh, size_t num_u, size_t num_v)
{
	status_t error = SUCCESS;

	double du = calc_du(num_u);
	double dv = calc_dv(num_v);
	S_EXTRACT(s1);
	S_EXTRACT(s2);
	S_EXTRACT(A);
	S_EXTRACT(B);
	S_EXTRACT(C);
	point3d_vec_t *points = mesh->points;
	double v = V_INIT;

	//Handle the lone point at the pole.
	IF_ERROR_GOTO(add_mesh_point(points, s1, s2, A, B, C, 0.0, v), error, exit0);

	//Use integral indices to avoid missing points due to floating point imprecision
	//Start at j = 1 because the first pole was 0, and stop at num_v - 1 because the
	//last pole will be num_v - 1.
	size_t j;
	for (j = 1, v += dv; j < num_v - 1; j++, v += dv)
	{
		size_t i;
		double u;
		//- 1 because the point at u == 0 is the same at u == 2pi
		for (i = 0, u = 0; i < num_u - 1; i++, u += du)
		{
			IF_ERROR_GOTO(add_mesh_point(points, s1, s2, A, B, C, u, v), error, exit0);
		}
	}

	//Handle the lone point at the opposite pole.
	v = -V_INIT;
	IF_ERROR_GOTO(add_mesh_point(points, s1, s2, A, B, C, 0.0, v), error, exit0);

	mesh->num_u = num_u;
	mesh->num_v = num_v;

exit0:
	return error;
}

status_t sellipsoid_calculate_mesh_normals(sellipsoid_t *sellipsoid, mesh_t *mesh)
{
	status_t error = SUCCESS;

	size_t num_u = mesh->num_u;
	size_t num_v = mesh->num_v;
	double du = calc_du(num_u);
	double dv = calc_dv(num_v);
	S_EXTRACT(A);
	S_EXTRACT(B);
	S_EXTRACT(C);
	S_EXTRACT(s1);
	S_EXTRACT(s2);
	point3d_vec_t *normals = mesh->normals;
	double v = V_INIT;

	//Handle the normal for the first pole point.
	IF_ERROR_GOTO(add_mesh_normal(normals, s1, s2, A, B, C, 0.0, v), error, exit0);

	//Use integral indices to avoid missing points due to floating point imprecision
	//Start at j = 1 because the first pole was 0, and stop at num_v - 1 because the
	//last pole will be num_v - 1.
	size_t j;
	for (j = 1, v += dv; j < num_v - 1; j++, v += dv)
	{
		size_t i;
		double u;
		//- 1 because the point at u == 0 is the same at u == 2pi
		for (i = 0, u = 0; i < num_u - 1; i++, u += du)
		{
			IF_ERROR_GOTO(add_mesh_normal(normals, s1, s2, A, B, C, u, v), error, exit0);
		}
	}

	//Handle the normal for the last pole point.
	v = -V_INIT;
	IF_ERROR_GOTO(add_mesh_normal(normals, s1, s2, A, B, C, 0.0, v), error, exit0);

exit0:
	return error;
}
