#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "awh44_math.h"
#include "bezier_surface.h"
#include "graphics.h"
#include "point3d.h"
#include "point3d_vec.h"
#include "status.h"

typedef struct
{
	char *filename;
	long num_u;
	long num_v;
	double radius;
	uint8_t use_flat;
} args_t;

status_t parse_args(int argc, char **argv, args_t *args);
void usage(char *prog);
void print_to_iv(bezier_surface_t *surface, point3d_vec_t *points);

int main(int argc, char **argv)
{
	status_t error = SUCCESS;
	
	args_t args;
	if ((error = parse_args(argc, argv, &args)))
	{
		usage(argv[0]);
		goto exit0;
	}

	FILE *file;
	if ((file = fopen(args.filename, "r"))  == NULL)
	{
		fprintf(stderr, "ERROR: could not open file %s\n", args.filename);
		error = FILE_OPEN_ERROR;
		goto exit0;
	}

	bezier_surface_t *bezier;
	if ((bezier = bezier_surface_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit1;
	}

	if ((error = read_points(file, bezier->ctrls)))
	{
		goto exit2;
	}

	if (point3d_vec_size(bezier->ctrls) != 16)
	{
		fprintf(stderr, "ERROR: bicubic Bezier patches must have 16 control points.\n");
		error = FILE_FORMAT_ERROR;
		goto exit2;
	}

	double du = 1 / (((double) args.num_u) - 1);
	double dv = 1 / (((double) args.num_v) - 1);

	point3d_vec_t *points;
	if ((points = point3d_vec_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit2;
	}

	if ((error = bezier_surface_calculate_mesh(bezier, points, du, dv)))
	{
		goto exit3;
	}

	print_to_iv(bezier, points);

	size_t i;
exit3:
	for (i = 0; i < point3d_vec_size(points); i++)
	{
		point3d_uninitialize(point3d_vec_get(points, i));
	}
	point3d_vec_uninitialize(points);
exit2:
	bezier_surface_uninitialize(bezier);
exit1:
	fclose(file);
exit0:
	return error;
}

status_t parse_args(int argc, char **argv, args_t *args)
{
	args->filename = "patchPoints.txt";
	args->num_u = 11;
	args->num_v = 11;
	args->radius = 1.0;
	args->use_flat = 1;

	uint8_t seen_S = 0;
	uint8_t seen_F = 0;

	char opt;
	while ((opt = getopt(argc, argv, "f:u:v:r:F:S:")) > 0)
	{
		switch (opt)
		{
			case 'f':
			{
				args->filename = optarg;
				break;
			}

			case 'u':
			{
				char *end;
				args->num_u = strtol(optarg, &end, 10);
				if (args->num_u < 2 || *end != '\0')
				{
					return ARGS_ERROR;
				}
				break;
			}

			case 'v':
			{
				char *end;
				args->num_v = strtol(optarg, &end, 10);
				if (args->num_v < 2 || *end != '\0')
				{
					return ARGS_ERROR;
				}
				break;
			}

			case 'r':
			{
				char *end;
				args->radius = strtod(optarg, &end);
				if (*end != '\0')
				{
					return ARGS_ERROR;
				}
				break;
			}

			case 'F':
			{
				if (seen_S)
				{
					return ARGS_ERROR;
				}
				args->use_flat = 1;
			}

			case 'S':
			{
				if (seen_F)
				{
					return ARGS_ERROR;
				}
				args->use_flat = 0;
			}

			case '?':
			{
				return ARGS_ERROR;
			}
		}
	}

	return optind == argc ? SUCCESS : ARGS_ERROR;
}

void usage(char *prog)
{
	fprintf(stderr,
		"usage: %s [-f filename] "
		"[-u 1 < number of u samples] [-v 1 < number of v samples] "
		"[-r control sphere radius]\n", prog);
}

void print_to_iv(bezier_surface_t *bezier, point3d_vec_t *points)
{
	printf("#Inventor V2.0 ascii\n");
	bezier_surface_print_to_iv(bezier, 0.1, stdout);

	size_t i;
	for (i = 0; i < point3d_vec_size(points); i++)
	{
		point3d_print_to_iv(point3d_vec_get(points, i), stdout, 0.1);
	}
}
