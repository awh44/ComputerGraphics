#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "matrix.h"
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

	matrix_t *c;
	if ((c = matrix_initialize(3, 3)) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	matrix_t *a;
	if ((a = matrix_initialize(3, 2)) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit1;
	}

	matrix_t *b;
	if ((b = matrix_initialize(2, 3)) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit2;
	}

	double a_arr[] = { 1, 4, 2, 5, 3, 6 };
	double b_arr[] = { 7, 9, 11, 8, 10, 12 };

	matrix_assign_from_array(a, a_arr);
	matrix_assign_from_array(b, b_arr);

	matrix_multiply(c, a, b);
	matrix_print(c, stderr);

exit3:
	matrix_uninitialize(b);
exit2:
	matrix_uninitialize(a);
exit1:
	matrix_uninitialize(c);
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
