#include <math.h>
#include <stdint.h>
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

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius);
void usage(char *prog);
status_t read_points(char *filename, point3d_vec_t *points);
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

	bezier_t *bezier;
	if ((bezier = bezier_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	if ((error = read_points(filename, bezier->ctrl)))
	{
		goto exit1;
	}

	polyline_t *poly;
	if ((poly = polyline_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit1;
	}

	if ((error = bezier_calculate_polyline(bezier, poly, u_inc)))
	{
		fprintf(stderr, "ERROR: Could not calculate the points to draw.\n");
		goto exit2;
	}

	print_to_iv(bezier, radius, poly);

exit2:
	polyline_uninitialize(poly);
exit1:
	bezier_uninitialize(bezier);
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

status_t read_points(char *filename, point3d_vec_t *points)
{
	status_t error = SUCCESS;

	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		fprintf(stderr, "ERROR: could not open file %s\n", filename);
		error = FILE_OPEN_ERROR;
		goto exit0;
	}

	char *line = NULL;
	size_t size = 0;
	ssize_t chars_read;

	while ((chars_read = getline(&line, &size, file)) > 0)
	{
		double coords[3];
		size_t elems = sizeof(coords) / sizeof(*coords);
		char *start = line;
		char *end;
		size_t i;
		for (i = 0; i < elems; i++)
		{
			coords[i] = strtod(start, &end);
			if (((i < elems - 1) && (*end != ' ')) ||
			    ((i == elems - 1) && (*end != '\n')))
			{
				fprintf(stderr, "ERROR: incorrect formatting in line %s", line);
				error = FILE_FORMAT_ERROR;
				goto exit1;
			}

			start = end + 1;
		}

		point3d_t *point = point3d_initialize_with_coords(coords[0], coords[1], coords[2]);
		point3d_vec_push_back(points, point);
	}

	if (!feof(file))
	{
		fprintf(stderr, "ERROR: could not read from file %s\n", filename);
		error = FILE_READ_ERROR;
		goto exit1;
	}

exit1:
	free(line);
	fclose(file);
exit0:
	return error;
}

void print_to_iv(bezier_t *bezier, double radius, polyline_t *poly)
{
	printf("#Inventor V2.0 ascii\n");
	bezier_print_to_iv(bezier, radius, stdout);
	polyline_print_to_iv(poly, stdout);
}
