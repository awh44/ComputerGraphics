#include "hierarchical.h"

#include "matrix.h"
#include "status.h"

status_t hierarchical_draw(hierarchical_t *model, matrix_t *transform)
{
	status_t error = SUCCESS;
	if (model == NULL)
	{
		goto exit0;
	}

	matrix_t *new_transform;
	INITIALIZE_OR_OUT_OF_MEM(new_transform, matrix_initialize(4, 4), error, exit0);

	matrix_multiply(new_transform, transform, model->from_parent);

	model->draw(&model->model, new_transform);

	if (model->child != NULL)
	{
		IF_ERROR_GOTO(hierarchical_draw(model->child, new_transform), error, exit1);
	}

	if (model->sibling != NULL)
	{
		IF_ERROR_GOTO(hierarchical_draw(model->sibling, transform), error, exit1);
	}

exit1:
	matrix_uninitialize(new_transform);
exit0:
	return error;
}
