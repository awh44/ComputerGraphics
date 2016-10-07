#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bezier.h"

#include "awh44_math.h"
#include "point3d_vec.h"
#include "polyline.h"
#include "status.h"

static status_t calculate_polyline_at_u(bezier_t *bezier, double u, point3d_t **draw);

bezier_t *bezier_initialize(void)
{
	bezier_t *bezier = malloc(sizeof *bezier);
	if (bezier == NULL)
	{
		return NULL;
	}

	bezier->ctrl = point3d_vec_initialize();
	if (bezier->ctrl == NULL)
	{
		free(bezier);
		return NULL;
	}

	return bezier;
}

void bezier_uninitialize(bezier_t *bezier)
{
	size_t num_ctrl = point3d_vec_size(bezier->ctrl);
	size_t i;
	for (i = 0; i < num_ctrl; i++)
	{
		point3d_uninitialize(point3d_vec_get(bezier->ctrl, i));
	}

	point3d_vec_uninitialize(bezier->ctrl);
	free(bezier);
}

status_t bezier_calculate_polyline(bezier_t *bezier, polyline_t *poly, double inc)
{
	status_t error = SUCCESS;
	double u;

	bezier_print_to_iv(bezier, .05, stdout);
	//The first point is just the first control point
	if ((error = polyline_copy_and_append_point(poly, point3d_vec_get(bezier->ctrl, 0))))
	{
		goto exit0;
	}

	for (u = inc; u < 1.0; u += inc)
	{
		point3d_t *new_point;
		if ((error = calculate_polyline_at_u(bezier, u, &new_point)))
		{
			goto exit0;
		}
		polyline_append_point(poly, new_point);
	}

	//Make sure to handle u == 1.0 - just thel last control point
	size_t last = point3d_vec_size(bezier->ctrl) - 1;
	if ((error = polyline_copy_and_append_point(poly, point3d_vec_get(bezier->ctrl, last))))
	{
		goto exit0;
	}

exit0:
	return error;

}

static status_t calculate_polyline_at_u(bezier_t *bezier, double u, point3d_t **draw)
{
	status_t error = SUCCESS;

	*draw = point3d_initialize();
	if (*draw == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	size_t k = point3d_vec_size(bezier->ctrl) - 1;
	size_t i;
	for (i = 0; i <= k; i++)
	{
		uint64_t combo = combination(k, i);
		double one_minus_u_pow = pow(1 - u, k - i);
		double u_pow = pow(u, i);
		double scalar = combo * one_minus_u_pow * u_pow;

		point3d_t *ctrl_point = point3d_vec_get(bezier->ctrl, i);
		(*draw)->x += ctrl_point->x * scalar;
		(*draw)->y += ctrl_point->y * scalar;
		(*draw)->z += ctrl_point->z * scalar;
	}

exit0:
	return error;
}

void bezier_print_to_iv(bezier_t *bezier, double radius, FILE *stream)
{
	size_t num = point3d_vec_size(bezier->ctrl);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_print_to_iv(point3d_vec_get(bezier->ctrl, i), stream, radius);
	}
}
