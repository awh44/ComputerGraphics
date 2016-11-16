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

status_t sellipsoid_calculate_mesh_points(sellipsoid_t *sellipsoid, mesh_t *mesh, size_t num_u, size_t num_v)
{
	status_t error = SUCCESS;
	double du = (2.0 * M_PI) / (((double) num_u) - 1.0);
	double dv = M_PI / (((double) num_v) - 1.0);

	double s1 = sellipsoid->s1;
	double s2 = sellipsoid->s2;
	double A = sellipsoid->A;
	double B = sellipsoid->B;
	double C = sellipsoid->C;

	point3d_vec_t *points = mesh->points;

	point3d_t *first_pole = point3d_initialize_with_coords(0, 0, C * s(-M_PI / 2, s1));
	point3d_vec_push_back(points, first_pole);

	double v;
	for (v = -M_PI / 2 + dv; v <= M_PI / 2 - dv; v += dv)
	{
		double u;
		for (u = -M_PI; u <= M_PI; u += du)
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
