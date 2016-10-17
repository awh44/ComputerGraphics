#include <stdio.h>
#include <unistd.h>

#include "catmullrom.h"
#include "graphics.h"
#include "polyline.h"

status_t parse_args(int argc, char **argv, char **filename, double *u_inc, double *radius);
void usage(char *prog);
status_t read_tangents(FILE *stream, point3d_t *t0, point3d_t *t1);
void print_to_iv(catmullrom_t *catmullrom, double radius, polyline_t *poly);

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

	catmullrom_t *catmullrom;
	if ((catmullrom = catmullrom_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit1;
	}

	if ((error = read_tangents(file, catmullrom->t0, catmullrom->tN)))
	{
		goto exit2;
	}

	if ((error = read_points(file, catmullrom->ctrl)))
	{
		goto exit2;
	}

	if (point3d_vec_size(catmullrom->ctrl) < 2)
	{
		fprintf(stderr, "ERROR: Catmull-Rom spline requires at least two points\n");
		error = FILE_FORMAT_ERROR;
		goto exit2;
	}

	polyline_t *poly;
	if ((poly = polyline_initialize()) == NULL)
	{
		error = OUT_OF_MEM;
		goto exit2;
	}

	if ((error = catmullrom_calculate_polyline(catmullrom, poly, u_inc)))
	{
		fprintf(stderr, "ERROR: Could not calculate the points to draw.\n");
		goto exit3;
	}

	print_to_iv(catmullrom, radius, poly);

exit3:
	polyline_uninitialize(poly);
exit2:
	catmullrom_uninitialize(catmullrom);
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

status_t read_tangents(FILE *stream, point3d_t *t0, point3d_t *t1)
{
	status_t error = SUCCESS;

	char *line = NULL;
	size_t size = 0;
	ssize_t chars_read;
	
	if ((chars_read = getline(&line, &size, stream)) <= 0)
	{
		error = FILE_READ_ERROR;
		goto exit0;
	}
	
	if ((error = parse_point(line, t0)))
	{
		goto exit0;
	}

	if ((chars_read = getline(&line, &size, stream)) <= 0)
	{
		error = FILE_READ_ERROR;
		goto exit0;
	}

	if ((error = parse_point(line, t1)))
	{
		goto exit0;
	}

exit0:
	free(line);
	return error;
}

void print_to_iv(catmullrom_t *catmullrom, double radius, polyline_t *poly)
{
	printf("#Inventor V2.0 ascii\n");
	catmullrom_print_to_iv(catmullrom, radius, stdout);
	polyline_print_to_iv(poly, stdout);
}
