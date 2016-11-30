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

#define POINT_SET(point, xn, yn, zn)\
	do\
	{\
		point->x = xn;\
		point->y = yn;\
		point->z = zn;\
	} while (0)

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

	point3d_t *points[2];
	INITIALIZE_OR_OUT_OF_MEM(points[0], point3d_initialize(), error, exit0);
	INITIALIZE_OR_OUT_OF_MEM(points[1], point3d_initialize(), error, exit1);

	cuboid_t *p0;
	INITIALIZE_OR_OUT_OF_MEM(p0, cuboid_initialize(), error, exit2);
	POINT_SET(points[0], -2, -2, 0);
	POINT_SET(points[1], 2, 2, 1);
	cuboid_set_corners(p0, points[0], points[1]);

	matrix_t *translate;
	INITIALIZE_OR_OUT_OF_MEM(translate, translation_matrix(0, 0, 1), error, exit3);
	matrix_t *rotate;
	INITIALIZE_OR_OUT_OF_MEM(rotate, rotation_matrix_z(args.theta1), error, exit4);

	matrix_t *m;
	INITIALIZE_OR_OUT_OF_MEM(m, matrix_initialize(4, 4), error, exit5);
	matrix_multiply(m, translate, rotate);

	matrix_t *p1pts[2];
	INITIALIZE_OR_OUT_OF_MEM
	(
		p1pts[0],
		matrix_initialize_with_array(4, 1, (double[]) { -0.5, -0.5, 0, 1 }),
		error, exit6
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		p1pts[1],
		matrix_initialize_with_array(4, 1, (double[]) { 0.5, 0.5, args.l1, 1 }),
		error, exit7
	);

	matrix_t *p1ptsnew[2];
	INITIALIZE_OR_OUT_OF_MEM(p1ptsnew[0], matrix_initialize(4, 1), error, exit8);
	INITIALIZE_OR_OUT_OF_MEM(p1ptsnew[1], matrix_initialize(4, 1), error, exit9);

	matrix_multiply(p1ptsnew[0], m, p1pts[0]);
	matrix_multiply(p1ptsnew[1], m, p1pts[1]);

	cuboid_t *p1;
	INITIALIZE_OR_OUT_OF_MEM(p1, cuboid_initialize(), error, exit3);
	for (size_t i = 0; i < sizeof(p1ptsnew) / sizeof(*p1ptsnew); i++)
	{
		POINT_SET
		(
			points[i],
			matrix_get(p1ptsnew[i], 0, 0),
			matrix_get(p1ptsnew[i], 1, 0),
			matrix_get(p1ptsnew[i], 2, 0)
		);
	}
	cuboid_set_corners(p1, points[0], points[1]);

	/*
	cuboid_t *p2;
	INITIALIZE_OR_OUT_OF_MEM(p2, cuboid_initialize(), error, exit4);
	POINT_SET(lowleft, -0.5, -0.5, 0);
	POINT_SET(upright, 0.5, 0.5, args.l2);
	cuboid_set_corners(p2, lowleft, upright);

	cuboid_t *p3;
	INITIALIZE_OR_OUT_OF_MEM(p3, cuboid_initialize(), error, exit5);
	POINT_SET(lowleft, -0.5, -0.5, 0);
	POINT_SET(upright, 0.5, 0.5, args.l3);
	cuboid_set_corners(p3, lowleft, upright);
	*/
	cuboid_print_to_iv(p0, stdout);
	cuboid_print_to_iv(p1, stdout);
	/*
	cuboid_print_to_iv(p2, stdout);
	cuboid_print_to_iv(p3, stdout);

exit6:
	cuboid_uninitialize(p3);
exit5:
	cuboid_uninitialize(p2);
exit4:
	cuboid_uninitialize(p1);
	*/
exit10:
	matrix_uninitialize(p1ptsnew[1]);
exit9:
	matrix_uninitialize(p1ptsnew[0]);
exit8:
	matrix_uninitialize(p1pts[1]);
exit7:
	matrix_uninitialize(p1pts[0]);
exit6:
	matrix_uninitialize(m);
exit5:
	matrix_uninitialize(rotate);
exit4:
	matrix_uninitialize(translate);
exit3:
	cuboid_uninitialize(p0);
exit2:
	point3d_uninitialize(points[1]);
exit1:
	point3d_uninitialize(points[0]);
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
