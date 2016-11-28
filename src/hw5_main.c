#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cuboid.h"
#include "matrix.h"
#include "point3d.h"
#include "status.h"

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

	cuboid_t *cuboid;
	INITIALIZE_OR_OUT_OF_MEM(cuboid, cuboid_initialize(), error, exit0);

	point3d_t *lowleft, *upright;
	INITIALIZE_OR_OUT_OF_MEM(lowleft, point3d_initialize_with_coords(-2, -2, 0), error, exit1);
	INITIALIZE_OR_OUT_OF_MEM(upright, point3d_initialize_with_coords(2, 2, 1), error, exit2);

	cuboid_set_corners(cuboid, lowleft, upright);

	cuboid_print_to_iv(cuboid, stdout);

exit3:
	point3d_uninitialize(upright);
exit2:
	point3d_uninitialize(lowleft);
exit1:
	cuboid_uninitialize(cuboid);
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

	return optind == argc ? SUCCESS : ARGS_ERROR;
}

void usage(char *prog)
{
	fprintf(stderr,
		"usage: %s\n"
		"	[-t theta 1] [-u theta 2] [-v theta 3]\n"
		"	[-l length 1] [-m length 2] [-n length 3]\n", prog);
}
