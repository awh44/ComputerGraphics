#include <stdio.h>
#include <stdlib.h>

#include "polyline.h"

polyline_t *polyline_initialize(void)
{
	polyline_t *polyline = malloc(sizeof *polyline);
	if (polyline == NULL)
	{
		return NULL;
	}

	polyline->points = point3d_vec_initialize();
	if (polyline->points == NULL)
	{
		free(polyline);
		return NULL;
	}

	return polyline;
}

void polyline_uninitialize(polyline_t *poly)
{
	size_t num_points = point3d_vec_size(poly->points);
	size_t i;
	for (i = 0; i < num_points; i++)
	{
		point3d_uninitialize(point3d_vec_get(poly->points, i));
	}

	point3d_vec_uninitialize(poly->points);
	free(poly);
}

void polyline_append_point(polyline_t *poly, point3d_t *point)
{
	point3d_vec_push_back(poly->points, point);
}

status_t polyline_copy_and_append_point(polyline_t *poly, point3d_t *point)
{
	status_t error = SUCCESS;
	point3d_t *copy = point3d_copy(point);
	if (copy == NULL)
	{
		error = OUT_OF_MEM;
		goto exit0;
	}

	polyline_append_point(poly, copy);

exit0:
	return error;
}

void polyline_print_to_iv(polyline_t *poly, FILE *stream)
{
	fprintf(stream,
"Separator {\n\
	LightModel {\n\
		model BASE_COLOR\n\
	}\n\
\n\
	Material {\n\
		diffuseColor 1.0 0.2 0.2\n\
	}\n\
\n\
	Coordinate3 {\n\
		point [\n");

	size_t num = point3d_vec_size(poly->points);
	size_t i;
	for (i = 0; i < num; i++)
	{
		point3d_t *point = point3d_vec_get(poly->points, i);
		fprintf(stream,
"			%lf %lf %lf,\n", point->x, point->y, point->z);
	}

	fprintf(stream,
"		]\n\
}\n\
	IndexedLineSet {\n\
		coordIndex [\n");

	for (i = 0; i < num; i++)
	{
		fprintf(stream,
"			%zu,\n", i);
	}

	fprintf(stream,
"			-1,\n\
		]\n\
	}\n\
}\n");
}
