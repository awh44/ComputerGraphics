#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "awh44_math.h"
#include "point3d.h"
#include "point3d_vec.h"
#include "status.h"

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius);
void usage(char *prog);

status_t read_points(char *filename, point3d_vec_t *points);
status_t calculate_draw_points(double u_inc, point3d_vec_t *ctrl, point3d_vec_t *draw);
status_t calculate_draw_point_at_u(point3d_vec_t *ctrl, double u, point3d_t **draw);
void print_to_iv(point3d_vec_t *ctrl, point3d_vec_t *draw, double radius);
void print_points_to_iv(point3d_vec_t *points, double radius);
void print_polyline_to_iv(point3d_vec_t *points);

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

	point3d_vec_t *ctrl_points = point3d_vec_initialize();
	if ((error = read_points(filename, ctrl_points)))
	{
		goto exit1;
	}

	point3d_vec_t *draw_points = point3d_vec_initialize();
	if ((error = calculate_draw_points(u_inc, ctrl_points, draw_points)))
	{
		fprintf(stderr, "ERROR: Could not calculate the points to draw.\n");
		goto exit2;
	}

	print_to_iv(ctrl_points, draw_points, radius);

exit2:
	point3d_vec_uninitialize_with_uninit(draw_points, point3d_uninitialize);
exit1:
	point3d_vec_uninitialize_with_uninit(ctrl_points, point3d_uninitialize);
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

status_t calculate_draw_points(double u_inc, point3d_vec_t *ctrl, point3d_vec_t *draw)
{
	status_t error = SUCCESS;
	double u;
	for (u = 0.0; u < 1.0; u += u_inc)
	{
		point3d_t *new_point;
		if ((error = calculate_draw_point_at_u(ctrl, u, &new_point)))
		{
			goto exit0;
		}
		point3d_vec_push_back(draw, new_point);
	}

	//Make sure to handle u == 1.0
	point3d_t *new_point;
	if ((error = calculate_draw_point_at_u(ctrl, 1.0, &new_point)))
	{
		goto exit0;
	}
	point3d_vec_push_back(draw, new_point);

exit0:
	return error;
}

status_t calculate_draw_point_at_u(point3d_vec_t *ctrl, double u, point3d_t **draw)
{
	status_t error = SUCCESS;

	*draw = point3d_initialize();
	if (*draw == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	size_t k = point3d_vec_size(ctrl) - 1;
	size_t i;
	for (i = 0; i <= k; i++)
	{
		uint64_t combo = combination(k, i);
		double one_minus_u_pow = pow(1 - u, k - i);
		double u_pow = pow(u, i);
		double scalar = combo * one_minus_u_pow * u_pow;

		point3d_t *ctrl_point = point3d_vec_get(ctrl, i);
		(*draw)->x += ctrl_point->x * scalar;
		(*draw)->y += ctrl_point->y * scalar;
		(*draw)->z += ctrl_point->z * scalar;
	}

exit0:
	return error;
}

void print_to_iv(point3d_vec_t *ctrl, point3d_vec_t *draw, double radius)
{
	printf("#Inventor V2.0 ascii\n");
	print_points_to_iv(ctrl, radius);
	print_polyline_to_iv(draw);
}

void print_points_to_iv(point3d_vec_t *points, double radius)
{
	size_t num = point3d_vec_size(points);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_print_to_iv(point3d_vec_get(points, i), stdout, radius);
	}
}

void print_polyline_to_iv(point3d_vec_t *points)
{
	printf(
"Separator {\n\
LightModel {\n\
model BASE_COLOR\n\
}\n\
Material {\n\
	diffuseColor 1.0 0.2 0.2\n\
}\n\
Coordinate3 {\n\
	point [\n");

	size_t num = point3d_vec_size(points);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_t *point = point3d_vec_get(points, i);
		printf(
"	%lf %lf %lf,\n", point->x, point->y, point->z);
	}

	printf(
"	]\n\
}\n\
IndexedLineSet {\n\
coordIndex [\n");

	for (i = 0; i < num; i++)
	{
		printf("%zu, ", i);
	}

	printf(
"-1,\n\
	]\n\
}\n\
}\n");
}
