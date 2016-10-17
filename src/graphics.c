#include <stdio.h>

#include "graphics.h"

status_t read_points(FILE *stream, point3d_vec_t *points)
{
	status_t error = SUCCESS;

	char *line = NULL;
	size_t size = 0;
	ssize_t chars_read;

	while ((chars_read = getline(&line, &size, stream)) > 0)
	{
		point3d_t *point = point3d_initialize();
		error = parse_point(line, point);
		if (error)
		{
			point3d_uninitialize(point);
			goto exit0;
		}
		point3d_vec_push_back(points, point);
	}

	if (!feof(stream))
	{
		fprintf(stderr, "ERROR: could not read from file\n");
		error = FILE_READ_ERROR;
		goto exit0;
	}

exit0:
	free(line);
	return error;
}

status_t parse_point(char *line, point3d_t *point)
{
	status_t error = SUCCESS;

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
			goto exit0;
		}

		start = end + 1;
	}

	point->x = coords[0];
	point->y = coords[1];
	point->z = coords[2];

exit0:
	return error;
}
