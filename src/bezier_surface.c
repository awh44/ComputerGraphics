#include <math.h>
#include <stdint.h>

#include "bezier_surface.h"

#include "awh44_math.h"
#include "point3d.h"
#include "point3d_vec.h"

bezier_surface_t *bezier_surface_initialize(void)
{
	bezier_surface_t *surface;
	if ((surface = malloc(sizeof *surface)) == NULL)
	{
		goto error0;
	}

	if ((surface->ctrls = point3d_vec_initialize()) == NULL)
	{
		goto error1;
	}

	goto success;

error1:
	free(surface);
	surface = NULL;
error0:

success:
	return surface;
}

void bezier_surface_uninitialize(bezier_surface_t *surface)
{
	size_t num_ctrl = point3d_vec_size(surface->ctrls);
	size_t i;
	for (i = 0; i < num_ctrl; i++)
	{
		point3d_uninitialize(point3d_vec_get(surface->ctrls, i));
	}

	point3d_vec_uninitialize(surface->ctrls);
	free(surface);
}

status_t bezier_surface_calculate_mesh_points(bezier_surface_t *surface, mesh_t *mesh, size_t num_u, size_t num_v)
{
	status_t error = SUCCESS;

	double du = 1 / (((double) num_u) - 1);
	double dv = 1 / (((double) num_v) - 1);

	point3d_vec_t *ctrls = surface->ctrls;
	point3d_vec_t *points = mesh->points;

	double u;
	for (u = 0.0; u <= 1.0; u += du)
	{
		double v;
		for (v = 0.0; v <= 1.0; v += dv)
		{
			point3d_t *new_point = point3d_initialize();
			if (new_point == NULL)
			{
				error = OUT_OF_MEM;
				goto exit0;
			}

			size_t j;
			for (j = 0; j < 4; j++)
			{
				double bernstein_m_j = bernstein_polynomial(3, j, v);

				size_t i;
				for (i = 0; i < 4; i++)
				{
					double bernstein_n_i = bernstein_polynomial(3, i, u);
					double scalar = bernstein_n_i * bernstein_m_j;

					point3d_t *ctrl = point3d_vec_get(ctrls, i + 4 * j);
					point3d_fmad(new_point, ctrl, scalar);
				}
			}

			point3d_vec_push_back(points, new_point);
		}
	}

	mesh->num_u = num_u;
	mesh->num_v = num_v;

exit0:
	return error;
}

static status_t calculate_partial_u(bezier_surface_t *bezier, double u, double v, point3d_t *result)
{
	status_t error = SUCCESS;
	double bernsteins[4] =
	{
		bernstein_polynomial(3, 0, v),
		bernstein_polynomial(3, 1, v),
		bernstein_polynomial(3, 2, v),
		bernstein_polynomial(3, 3, v),
	};

	double scalars_for_partial[4] =
	{
		-3.0 * pow((1.0 - u), 2.0),
		3.0 * pow((1.0 - u), 2.0) - 6.0 * u * (1.0 - u),
		6.0 * u * (1.0 - u) - 3.0 * u * u,
		3.0 * u * u,
	};


	size_t i;
	point3d_vec_t *ctrls = bezier->ctrls;
	for (i = 0; i < 4; i++)
	{
		point3d_t *temp = point3d_initialize();
		if (temp == NULL)
		{
			error = OUT_OF_MEM;
			goto exit0;
		}

		size_t j;
		for (j = 0; j < 4; j++)
		{
			point3d_t *point = point3d_vec_get(ctrls, i * 4 + j);
			point3d_fmad(temp, point, bernsteins[j]);
		}

		point3d_scale(temp, scalars_for_partial[i]);
		point3d_add(result, temp);
		point3d_uninitialize(temp);
	}

exit0:
	return error;
}

static status_t calculate_partial_v(bezier_surface_t *bezier, double u, double v, point3d_t *result)
{
	status_t error = SUCCESS;
	double bernsteins[4] =
	{
		bernstein_polynomial(3, 0, u),
		bernstein_polynomial(3, 1, u),
		bernstein_polynomial(3, 2, u),
		bernstein_polynomial(3, 3, u),
	};

	double scalars_for_partial[4] =
	{
		-3.0 * pow((1.0 - v), 2.0),
		3.0 * pow((1.0 - v), 2.0) - 6.0 * v * (1.0 - v),
		6.0 * v * (1.0 - v) - 3.0 * v * v,
		3.0 * v * v,
	};


	size_t j;
	point3d_vec_t *ctrls = bezier->ctrls;
	for (j = 0; j < 4; j++)
	{
		point3d_t *temp = point3d_initialize();
		if (temp == NULL)
		{
			error = OUT_OF_MEM;
			goto exit0;
		}

		size_t i;
		for (i = 0; i < 4; i++)
		{
			point3d_t *point = point3d_vec_get(ctrls, i * 4 + j);
			point3d_fmad(temp, point, bernsteins[i]);
		}

		point3d_scale(temp, scalars_for_partial[j]);
		point3d_add(result, temp);
		point3d_uninitialize(temp);
	}

exit0:
	return error;
}


status_t bezier_surface_calculate_mesh_normals(bezier_surface_t *bezier, mesh_t *mesh)
{
	status_t error = SUCCESS;

	double du = 1 / (((double) mesh->num_u) - 1);
	double dv = 1 / (((double) mesh->num_v) - 1);

	double u;
	for (u = 0; u <= 1.0; u += du)
	{
		double v;
		for (v = 0; v <= 1.0; v += dv)
		{
			point3d_t *partial_u;
			point3d_t *partial_v;
			if ((partial_u = point3d_initialize()) == NULL)
			{
				error = OUT_OF_MEM;
				goto loop_exit0;
			}

			if ((partial_v = point3d_initialize()) == NULL)
			{
				error = OUT_OF_MEM;
				goto loop_exit1;
			}

			if ((error = calculate_partial_u(bezier, u, v, partial_u)) ||
			    (error = calculate_partial_v(bezier, u, v, partial_v)))
			{
				goto loop_exit2;
			}

			point3d_t *normal;
			if ((normal = point3d_initialize()) == NULL)
			{
				goto loop_exit2;
			}

			normal->x = partial_u->y * partial_v->z - partial_v->y * partial_u->z;
			normal->y = partial_u->z * partial_v->x - partial_v->z * partial_u->x;
			normal->z = partial_u->x * partial_v->y - partial_v->x * partial_u->y;

			point3d_vec_push_back(mesh->normals, normal);

loop_exit2:
			point3d_uninitialize(partial_v);
loop_exit1:
			point3d_uninitialize(partial_u);
loop_exit0:
			if (error)
			{
				goto exit0;
			}
		}
	}

exit0:
	return error;
}

void bezier_surface_print_to_iv(bezier_surface_t *surface, double radius, FILE *stream)
{
	size_t num = point3d_vec_size(surface->ctrls);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_print_to_iv(point3d_vec_get(surface->ctrls, i), stream, radius);
	}
}
