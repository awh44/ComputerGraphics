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

status_t bezier_surface_calculate_mesh_points(bezier_surface_t *surface, mesh_t *mesh, double du, double dv)
{
	status_t error = SUCCESS;

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
