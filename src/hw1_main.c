#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "awh44_math.h"
#include "bezier.h"
#include "point3d.h"
#include "point3d_vec.h"
#include "polyline.h"
#include "status.h"

#include "graphics.h"

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius);
void usage(char *prog);
void print_to_iv(bezier_t *bezier, double radius, polyline_t *poly);

int main(int argc, char **argv)
{
	status_t error = SUCCESS;
	char *filename;
	double u_inc;
	double radius;
	if ((error = parse_args(argc, argv, &filename, &u_inc, &radius)))
	{
		usage(argv[0]);
		goto exit0;
	}

	FILE *file;
	if ((file = fopen(filename, "r"))  == NULL)
	{
		fprintf(stderr, "ERROR: could not open file %s\n", filename);
		error = FILE_OPEN_ERROR;
		goto exit0;
	}

	bezier_t *bezier;
	if ((bezier = bezier_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit1;
	}

	if ((error = read_points(file, bezier->ctrl)))
	{
		goto exit2;
	}

	polyline_t *poly;
	if ((poly = polyline_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit2;
	}

	if ((error = bezier_calculate_polyline(bezier, poly, u_inc)))
	{
		fprintf(stderr, "ERROR: Could not calculate the points to draw.\n");
		goto exit3;
	}

	print_to_iv(bezier, radius, poly);

exit3:
	polyline_uninitialize(poly);
exit2:
	bezier_uninitialize(bezier);
exit1:
	fclose(file);
exit0:
	return error;
}

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius)
{
	*filename = "cpts_in.txt";
	*u_inc = .09;
	*radius = 0.1;

	char opt;
	while ((opt = getopt(argc, argv, "f:u:r:")) > 0)
	{
		switch (opt)
		{
			case 'f':
			{
				*filename = optarg;
				break;
			}

			case 'u':
			{
				char *end;
				*u_inc = strtod(optarg, &end);
				if (*u_inc <= 0.0 || *u_inc > 1.0)
				{
					return ARGS_ERROR;
				}
				break;
			}

			case 'r':
			{
				char *end;
				*radius = strtod(optarg, &end);
				break;
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
		"usage: %s [-f filename] [-u 0.0 < increment < 1.0 ] [-r sphere radius]\n", prog);
}

void print_to_iv(bezier_t *bezier, double radius, polyline_t *poly)
{
	printf("#Inventor V2.0 ascii\n");
	bezier_print_to_iv(bezier, radius, stdout);
	polyline_print_to_iv(poly, stdout);
}
