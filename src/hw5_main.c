#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "awh44_math.h"
#include "cuboid.h"
#include "hierarchal.h"
#include "matrix.h"
#include "point3d.h"
#include "status.h"
#include "transforms.h"

typedef struct
{
	double theta1, theta2, theta3;
	double l1, l2, l3;
} args_t;

status_t parse_args(int argc, char **argv, args_t *args);
void usage(char *prog);

void point_draw(model_t *model, matrix_t *transform)
{
	matrix_t *tmp = matrix_initialize(4, 1);
	matrix_multiply(tmp, transform, model->matrices[0]);
	matrix_assign(model->matrices[0], tmp);
	matrix_uninitialize(tmp);
	point3d_print_matrix_to_iv(model->matrices[0], stdout, 0.2);
}

status_t point_model_initialize(hierarchal_t *model, double *loc, double *pt)
{
	status_t error = SUCCESS;

	INITIALIZE_OR_OUT_OF_MEM(model->model.matrices, malloc(sizeof *model->model.matrices), error, error0);

	IF_ERROR_GOTO
	(
		point3d_initialize_matrix(model->model.matrices, (double[]) { 0.0, 0.0, 0.0 }),
		error, error1;
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		model->from_parent,
		translation_matrix(pt[0], pt[1], pt[2]),
		error, error2
	);

	model->draw = point_draw;
	model->sibling = NULL;
	model->child = NULL;

	goto success;

error2:
	matrix_uninitialize(model->model.matrices[0]);
error1:
	free(model->model.matrices);
error0:

success:
	return error;
}

void point_model_uninitialize(hierarchal_t *model)
{
	matrix_uninitialize(model->model.matrices[0]);
	free(model->model.matrices);
	matrix_uninitialize(model->from_parent);
}

void cuboid_draw(model_t *model, matrix_t *transform)
{
	matrix_t *tmp = matrix_initialize(4, 1);
	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_multiply(tmp, transform, model->matrices[i]);
		matrix_assign(model->matrices[i], tmp);
	}
	matrix_uninitialize(tmp);
	cuboid_print_matrices_to_iv(model->matrices, stdout);
}

status_t cuboid_model_initialize(hierarchal_t *model, double *ll, double *ur, double *pt, double pr, rotatedir_t dir)
{
	status_t error = SUCCESS;

	INITIALIZE_OR_OUT_OF_MEM(model->model.matrices, malloc(CUBOID_POINTS * sizeof *model->model.matrices), error, error0);

	IF_ERROR_GOTO(cuboid_initialize_matrices(model->model.matrices, ll, ur), error, error1);
	INITIALIZE_OR_OUT_OF_MEM(model->from_parent, matrix_initialize(4, 4), error, error2);

	matrix_t *translate;
	INITIALIZE_OR_OUT_OF_MEM(translate, translation_matrix(pt[0], pt[1], pt[2]), error, error2);

	matrix_t *rotate;
	if ((rotate = rotation_matrix(pr, dir)) == NULL)
	{
		matrix_uninitialize(translate);
		error = OUT_OF_MEM;
		goto error2;
	}

	matrix_multiply(model->from_parent, translate, rotate);
	matrix_uninitialize(translate);
	matrix_uninitialize(rotate);

	model->draw = cuboid_draw;
	model->sibling = NULL;
	model->child = NULL;

	goto success;

error2:
	cuboid_uninitialize_matrices(model->model.matrices);
error1:
	free(model->model.matrices);
error0:

success:
	return error;
}

void cuboid_model_uninitialize(hierarchal_t *model)
{
	cuboid_uninitialize_matrices(model->model.matrices);
	free(model->model.matrices);
	matrix_uninitialize(model->from_parent);
}

int main(int argc, char **argv)
{
	status_t error = SUCCESS;

	args_t args;
	if ((error = parse_args(argc, argv, &args)))
	{
		usage(argv[0]);
		goto exit0;
	}

	double ll[4][3] =
	{
		{ -2, -2, 0 },
		{ -.5, -.5, 0 },
		{ -.5, -.5, 0 },
		{ -.5, -.5, 0 },

	};

	double ur[4][3] =
	{
		{ 2, 2, 1 },
		{ .5, .5, args.l1 },
		{ .5, .5, args.l2 },
		{ .5, .5, args.l3 },
	};

	double trans[4][3] =
	{
		{ 0, 0, 0 },
		{ 0, 0, ur[0][2] },
		{ 0, 0, ur[1][2] },
		{ 0, 0, ur[2][2] },
	};

	hierarchal_t models[6];

	IF_ERROR_GOTO
	(
		point_model_initialize(models + 0, (double[]) { 0.0, 0.0, 0.0 }, (double[]) { 0.0, 0.0, 0.0 }), error, exit0
	);
	models[0].child = models + 1;

	IF_ERROR_GOTO
	(
		cuboid_model_initialize(models + 1, ll[0], ur[0], trans[0], 0, ROTATE_X), error, exit1
	);
	models[1].child = models + 2;

	IF_ERROR_GOTO
	(
		cuboid_model_initialize(models + 2, ll[1], ur[1], trans[1], args.theta1, ROTATE_Z), error, exit2
	);
	models[2].child = models + 3;

	IF_ERROR_GOTO
	(
		cuboid_model_initialize(models + 3, ll[2], ur[2], trans[2], args.theta2, ROTATE_Y), error, exit3
	);
	models[3].child = models + 4;

	IF_ERROR_GOTO
	(
		cuboid_model_initialize(models + 4, ll[3], ur[3], trans[3], args.theta3, ROTATE_Y), error, exit4
	);
	models[4].child = models + 5;

	IF_ERROR_GOTO
	(
		point_model_initialize(models + 5, (double[]) { 0.0, 0.0, 0.0 }, (double[]) { 0.0, 0.0, ur[3][2] }), error, exit5
	);

	matrix_t *initial_transform;
	INITIALIZE_OR_OUT_OF_MEM(initial_transform, translation_matrix(0, 0, 0), error, exit6);
	IF_ERROR_GOTO(hierarchal_draw(models + 0, initial_transform), error, exit7);

exit7:
	matrix_uninitialize(initial_transform);
exit6:
	point_model_uninitialize(models + 5);
exit5:
	cuboid_model_uninitialize(models + 4);
exit4:
	cuboid_model_uninitialize(models + 3);
exit3:
	cuboid_model_uninitialize(models + 2);
exit2:
	cuboid_model_uninitialize(models + 1);
exit1:
	point_model_uninitialize(models + 0);
exit0:
	return error;
}

status_t parse_args(int argc, char **argv, args_t *args)
{
	args->theta1 = -51.0;
	args->theta2 = 39.0;
	args->theta3 = 65.0;
	args->l1 = 4.0;
	args->l2 = 3.0;
	args->l3 = 2.5;

	char opt;
	while ((opt = getopt(argc, argv, "t:u:v:l:m:n:")) > 0)
	{
		double *arg;
		switch (opt)
		{
			case 't':
				arg = &args->theta1;
				break;
			case 'u':
				arg = &args->theta2;
				break;
			case 'v':
				arg = &args->theta3;
				break;
			case 'l':
				arg = &args->l1;
				break;
			case 'm':
				arg = &args->l2;
				break;
			case 'n':
				arg = &args->l3;
				break;
			case '?':
			default:
				return ARGS_ERROR;
		}

		char *end;
		*arg = strtod(optarg, &end);
		if (*end != '\0')
		{
			return ARGS_ERROR;
		}
	}

	//Fix up the angles to be in radians so that, after parsing the arguments,
	//they can just be treated straightforwardly as the correct angles, always
	args->theta1 = TO_RAD(args->theta1);
	args->theta2 = TO_RAD(args->theta2);
	args->theta3 = TO_RAD(args->theta3);

	return optind == argc ? SUCCESS : ARGS_ERROR;
}

void usage(char *prog)
{
	fprintf(stderr,
		"usage: %s\n"
		"	[-t theta 1] [-u theta 2] [-v theta 3]\n"
		"	[-l length 1] [-m length 2] [-n length 3]\n", prog);
}
