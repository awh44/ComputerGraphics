#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "awh44_math.h"
#include "cuboid.h"
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

int main(int argc, char **argv)
{
	status_t error = SUCCESS;

	args_t args;
	if ((error = parse_args(argc, argv, &args)))
	{
		usage(argv[0]);
		goto exit0;
	}

	matrix_t *p0pts[CUBOID_POINTS];
	double ll0[] = { -2, -2, 0 };
	double ur0[] = { 2, 2, 1 };
	IF_ERROR_GOTO(cuboid_initialize_matrices(p0pts, ll0, ur0), error, exit0);
	cuboid_print_matrices_to_iv(p0pts, stdout);

	matrix_t *translate;
	INITIALIZE_OR_OUT_OF_MEM(translate, translation_matrix(0, 0, 1), error, exit1);
	matrix_t *rotate;
	INITIALIZE_OR_OUT_OF_MEM(rotate, rotation_matrix_z(args.theta1), error, exit2);
	matrix_t *m;
	INITIALIZE_OR_OUT_OF_MEM(m, matrix_initialize(4, 4), error, exit3);
	matrix_multiply(m, translate, rotate);

	matrix_t *p1pts[CUBOID_POINTS];
	double ll1[] = { -0.5, -0.5, 0 };
	double ur1[] = { 0.5, 0.5, args.l1 };
	IF_ERROR_GOTO(cuboid_initialize_matrices(p1pts, ll1, ur1), error, exit4);

	matrix_t *tmpvector;
	INITIALIZE_OR_OUT_OF_MEM(tmpvector, matrix_initialize(4, 1), error, exit5);
	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_multiply(tmpvector, m, p1pts[i]);
		matrix_assign(p1pts[i], tmpvector);
	}
	cuboid_print_matrices_to_iv(p1pts, stdout);

	translation_matrix_assign(translate, 0, 0, args.l1);
	rotation_matrix_y_assign(rotate, args.theta2);

	matrix_t *tmpmatrix1, *tmpmatrix2;
	INITIALIZE_OR_OUT_OF_MEM(tmpmatrix1, matrix_initialize(4, 4), error, exit6);
	INITIALIZE_OR_OUT_OF_MEM(tmpmatrix2, matrix_initialize(4, 4), error, exit7);

	matrix_multiply(tmpmatrix1, translate, rotate);
	matrix_multiply(tmpmatrix2, m, tmpmatrix1);
	matrix_assign(m, tmpmatrix2);

	matrix_t *p2pts[CUBOID_POINTS];
	double ll2[] = { -0.5, -0.5, 0 };
	double ur2[] = { 0.5, 0.5, args.l2 };
	IF_ERROR_GOTO(cuboid_initialize_matrices(p2pts, ll2, ur2), error, exit8);

	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_multiply(tmpvector, m, p2pts[i]);
		matrix_assign(p2pts[i], tmpvector);
	}
	cuboid_print_matrices_to_iv(p2pts, stdout);

exit9:
	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_uninitialize(p2pts[i]);
	}
exit8:
	matrix_uninitialize(tmpmatrix2);
exit7:
	matrix_uninitialize(tmpmatrix1);
exit6:
	matrix_uninitialize(tmpvector);
exit5:
	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_uninitialize(p1pts[i]);
	}
exit4:
	matrix_uninitialize(m);
exit3:
	matrix_uninitialize(rotate);
exit2:
	matrix_uninitialize(translate);
exit1:
	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_uninitialize(p0pts[i]);
	}
exit0:
	return error;
}

status_t parse_args(int argc, char **argv, args_t *args)
{
	args->theta1 = -51.0;
	args->theta2 = 39.0;
	args->theta3 = 65;
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
