#include <math.h>
#include <stdio.h>

#include "sellipsoid.h"

#include "awh44_math.h"
#include "mesh.h"
#include "point3d_vec.h"
#include "status.h"

static double either(double w, double m, double (*func)(double))
{
	double funcw = func(w);
	return sgn(funcw) * pow(fabs(funcw), m);
}

static double c(double w, double m)
{
	return either(w, m, cos);
}

static double s(double w, double m)
{
	return either(w, m, sin);
}

static double inline calc_du(size_t num_u)
{
	return (2.0 * M_PI) / (((double) num_u) - 1.0);
}

static double inline calc_dv(size_t num_v)
{
	return M_PI / (((double) num_v) - 1.0);
}

status_t sellipsoid_calculate_mesh_points(sellipsoid_t *sellipsoid, mesh_t *mesh, size_t num_u, size_t num_v)
{
	status_t error = SUCCESS;

	double du = calc_du(num_u);
	double dv = calc_dv(num_v);
	double s1 = sellipsoid->s1;
	double s2 = sellipsoid->s2;
	double A = sellipsoid->A;
	double B = sellipsoid->B;
	double C = sellipsoid->C;

	point3d_vec_t *points = mesh->points;

	point3d_t *first_pole = point3d_initialize_with_coords(0, 0, C * s(-M_PI / 2, s1));
	point3d_vec_push_back(points, first_pole);

	//Use integral indices to avoid missing points due to floating point imprecision
	size_t j;
	double v;
	for (j = 1, v = -M_PI / 2 + dv; j < num_v - 1; j++, v += dv)
	{
		size_t i;
		double u;
		for (i = 0, u = -M_PI; i < num_u; i++, u += du)
		{
			point3d_t *new_point;
			if ((new_point = point3d_initialize()) == NULL)
			{
				error = OUT_OF_MEM;
				goto exit0;
			}

			new_point->x = A * c(v, s1) * c(u, s2);
			new_point->y = B * c(v, s1) * s(u, s2);
			new_point->z = C * s(v, s1);

			point3d_vec_push_back(points, new_point);
		}
	}

	point3d_t *last_pole = point3d_initialize_with_coords(0, 0, C * s(M_PI / 2, s1));
	point3d_vec_push_back(points, last_pole);

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
	double two_min_s1 = 2 - sellipsoid->s1;
	double two_min_s2 = 2 - sellipsoid->s2;
	double one_over_A = 1 / sellipsoid->A;
	double one_over_B = 1 / sellipsoid->B;
	double one_over_C = 1 / sellipsoid->C;

	point3d_vec_t *normals = mesh->normals;

	point3d_t *first_norm = point3d_initialize_with_coords(0, 0, one_over_C * s(-M_PI / 2, two_min_s1));
	point3d_vec_push_back(normals, first_norm);

	size_t j;
	double v;
	for (j = 0, v = -M_PI / 2 + dv; j < num_v - 1; j++, v += dv)
	{
		size_t i;
		double u;
		for (i = 0, u = -M_PI; i < num_u; i++, u += du)
		{
			double x = one_over_A * c(v, two_min_s1) * c(u, two_min_s2);
			double y = one_over_B * c(v, two_min_s1) * s(u, two_min_s2);
			double z = one_over_C * s(v, two_min_s1);

			point3d_t *normal;
			if ((normal = point3d_initialize_with_coords(x, y, z)) == NULL)
			{
				error = OUT_OF_MEM;
				goto exit0;
			}

			point3d_vec_push_back(normals, normal);
		}
	}

	point3d_t *last_norm = point3d_initialize_with_coords(0, 0, one_over_C * s(M_PI / 2, two_min_s1));
	point3d_vec_push_back(normals, last_norm);

exit0:
	return error;
}
