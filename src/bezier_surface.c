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

static status_t calculate_partial(bezier_surface_t *bezier, double u, double v, uint8_t is_u_partial, point3d_t *result)
{
	/*
		a = -3 * (1 - u)^2
		b = 3 * (1 - u)^2 -6 * u * (1 - u)
		c = 6 * u * (1 - u) - 3 * u^2
		d = 3 * u^2

		d/du =
			a * (B(3, 0, v) * p[0] + B(3, 1, v) * p[1] + B(3, 2, v) * p[2] + B(3, 3, v) * p[4])
			b * (B(3, 0, v) * p[4] + B(3, 1, v) * p[5] + B(3, 2, v) * p[6] + B(3, 3, v) * p[7])
			c * (B(3, 0, v) * p[8] + B(3, 1, v) * p[9] + B(3, 2, v) * p[10] + B(3, 3, v) * p[11])
			d * (B(3, 0, v) * p[12] + B(3, 1, v) * p[13] + B(3, 2, v) * p[14] + B(3, 3, v) * p[15])

		d/dv =
			a * (B(3, 0, u) * p[0] + B(3, 1, u) * p[4] + B(3, 2, u) * p[8] + B(3, 3, u) * p[12])
			b * (B(3, 0, u) * p[1] + B(3, 1, u) * p[5] + B(3, 2, u) * p[9] + B(3, 3, u) * p[13])
			c * (B(3, 0, u) * p[2] + B(3, 1, u) * p[6] + B(3, 2, u) * p[10] + B(3, 3, u) * p[14])
			d * (B(3, 0, u) * p[3] + B(3, 1, u) * p[7] + B(3, 2, u) * p[11] + B(3, 3, u) * p[15])
	*/

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

	size_t i, j;
	//Use these pointers to track which index (i or j) to increment. As seen in the comment above,
	//the partial derivatives are identical, besides the order of the indices to be used for the points.
	//The pointers allow for the order in which the indices are traversed ("row" first or "column" first)
	//to be swapped.
	size_t *outer_index, *inner_index;
	if (is_u_partial)
	{
		outer_index = &i;
		inner_index = &j;
	}
	else
	{
		outer_index = &j;
		inner_index = &i;
	}

	point3d_vec_t *ctrls = bezier->ctrls;
	for (*outer_index = 0; *outer_index < 4; (*outer_index)++)
	{
		point3d_t *temp = point3d_initialize();
		if (temp == NULL)
		{
			error = OUT_OF_MEM;
			goto exit0;
		}

		for (*inner_index = 0; *inner_index < 4; (*inner_index)++)
		{
			point3d_t *point = point3d_vec_get(ctrls, i * 4 + j);
			point3d_fmad(temp, point, bernsteins[*inner_index]);
		}

		point3d_scale(temp, scalars_for_partial[*outer_index]);
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

			if ((error = calculate_partial(bezier, u, v, 1, partial_u)) ||
			    (error = calculate_partial(bezier, v, u, 0, partial_v)))
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
