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

	//Make sure to handle u == 1.0 - just the last control point
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
		double scalar = bernstein_polynomial(k, i, u);
		point3d_t *ctrl_point = point3d_vec_get(bezier->ctrl, i);
		point3d_fmad(*draw, ctrl_point, scalar);
	}

exit0:
	return error;
}

status_t bezier_from_hermite(bezier_t *bezier, point3d_t *p0, point3d_t *p3, point3d_t *t0, point3d_t *t1)
{
	status_t error = SUCCESS;
	point3d_t *p0_copy = point3d_copy(p0);
	point3d_t *p3_copy = point3d_copy(p3);
	if (p0_copy == NULL || p3_copy == NULL)
	{
		goto error0;
	}

	// p1 = p0 + 1/3 * t0
	point3d_t *p1 = point3d_copy(t0);
	if (p1 == NULL)
	{
		goto error1;
	}
	point3d_scale(p1, 1.0 / 3.0);
	point3d_add(p1, p0);

	// p2 = p3 - 1/3 * t1
	point3d_t *p2 = point3d_copy(t1);
	if (p2 == NULL)
	{
		goto error2;
	}
	point3d_scale(p2, -1.0 / 3.0);
	point3d_add(p2, p3);

	point3d_vec_push_back(bezier->ctrl, p0_copy);
	point3d_vec_push_back(bezier->ctrl, p1);
	point3d_vec_push_back(bezier->ctrl, p2);
	point3d_vec_push_back(bezier->ctrl, p3_copy);
	goto exit0;

error2:
	free(p2);
error1:
	free(p1);
error0:
	free(p3_copy);
	free(p0_copy);
	error = OUT_OF_MEM;

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
