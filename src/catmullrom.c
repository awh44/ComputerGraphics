#include <stdio.h>
#include <stdlib.h>

#include "catmullrom.h"
#include "bezier.h"
#include "point3d.h"
#include "point3d_vec.h"

catmullrom_t *catmullrom_initialize(void)
{
	catmullrom_t *catmullrom;
	if ((catmullrom = malloc(sizeof *catmullrom)) == NULL)
	{
		goto error0;
	}

	if ((catmullrom->ctrl = point3d_vec_initialize()) == NULL)
	{
		goto error1;
	}

	if ((catmullrom->t0 = point3d_initialize()) == NULL)
	{
		goto error2;
	}

	if ((catmullrom->tN = point3d_initialize()) == NULL)
	{
		goto error3;
	}

	goto exit0;

error3:
	point3d_uninitialize(catmullrom->t0);
error2:
	point3d_vec_uninitialize(catmullrom->ctrl);
error1:
	free(catmullrom);
	catmullrom = NULL;
error0:


exit0:
	return catmullrom;
}

void catmullrom_uninitialize(catmullrom_t *catmullrom)
{
	size_t num_ctrl = point3d_vec_size(catmullrom->ctrl);
	size_t i;
	for (i = 0; i < num_ctrl; i++)
	{
		point3d_uninitialize(point3d_vec_get(catmullrom->ctrl, i));
	}
	point3d_vec_uninitialize(catmullrom->ctrl);
	point3d_uninitialize(catmullrom->t0);
	point3d_uninitialize(catmullrom->tN);
	free(catmullrom);
}

status_t catmullrom_calculate_polyline(catmullrom_t *catmullrom, polyline_t *poly, double inc)
{
	status_t error = SUCCESS;
	point3d_vec_t *ctrl = catmullrom->ctrl;
	size_t num_ctrl = point3d_vec_size(ctrl);

	point3d_t *t0 = point3d_copy(catmullrom->t0);
	if (t0 == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	size_t k;
	for (k = 0; k < num_ctrl - 2; k++)
	{
		point3d_t *pk = point3d_vec_get(ctrl, k);
		point3d_t *pk_plus1 = point3d_vec_get(ctrl, k + 1);
		point3d_t *pk_plus2 = point3d_vec_get(ctrl, k + 2);

		//t1 = 0.5 * (pk+2 - pk)
		point3d_t *t1;
		if ((t1 = point3d_copy(pk_plus2)) == NULL)
		{
			error = OUT_OF_MEM;
			goto loop_error0;
		}
		point3d_sub(t1, pk);
		point3d_scale(t1, 0.5);

		bezier_t *bezier;
		if ((bezier = bezier_initialize()) == NULL)
		{
			error = OUT_OF_MEM;
			goto loop_error1;
		}

		if ((error = bezier_from_hermite(bezier, pk, pk_plus1, t0, t1)))
		{
			goto loop_error2;
		}

		if ((error = bezier_calculate_polyline(bezier, poly, inc)))
		{
			goto loop_error2;
		}

		point3d_uninitialize(t0);
		t0 = t1;
		bezier_uninitialize(bezier);
		continue;

loop_error2:
		bezier_uninitialize(bezier);
loop_error1:
		point3d_uninitialize(t1);
loop_error0:
		break;
	}

	bezier_t *bezier;
	if ((bezier = bezier_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	error = bezier_from_hermite(bezier,
		point3d_vec_get(ctrl, num_ctrl - 2),
		point3d_vec_get(ctrl, num_ctrl - 1),
		t0,
		catmullrom->tN);
	if (error)
	{
		goto exit1;
	}

	if ((error = bezier_calculate_polyline(bezier, poly, inc)))
	{
		goto exit1;
	}

exit1:
	bezier_uninitialize(bezier);
exit0:
	point3d_uninitialize(t0);
	return error;
}

void catmullrom_print_to_iv(catmullrom_t *catmullrom, double radius, FILE *stream)
{
	size_t num = point3d_vec_size(catmullrom->ctrl);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_print_to_iv(point3d_vec_get(catmullrom->ctrl, i), stream, radius);
	}
}
