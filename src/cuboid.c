#include <stdlib.h>

#include "cuboid.h"
#include "point3d.h"
#include "status.h"

cuboid_t *cuboid_initialize(void)
{
	cuboid_t *cuboid;
	if ((cuboid = malloc(sizeof *cuboid)) == NULL)
	{
		goto error0;
	}

	if ((cuboid->lowleft = point3d_initialize()) == NULL)
	{
		goto error1;
	}

	if ((cuboid->upright = point3d_initialize()) == NULL)
	{
		goto error2;
	}

	goto success;

error2:
	point3d_uninitialize(cuboid->lowleft);
error1:
	free(cuboid);
	cuboid = NULL;
error0:

success:
	return cuboid;
}

void cuboid_uninitialize(cuboid_t *cuboid)
{
	point3d_uninitialize(cuboid->lowleft);
	point3d_uninitialize(cuboid->upright);
	free(cuboid);
}

void cuboid_set_corners(cuboid_t *cuboid, point3d_t *lowleft, point3d_t *upright)
{
	point3d_assign(cuboid->lowleft, lowleft);
	point3d_assign(cuboid->upright, upright);
}

void cuboid_print_to_iv(cuboid_t *cuboid, FILE *stream)
{
	point3d_t *lowleft = cuboid->lowleft;
	double llx = lowleft->x;
	double lly = lowleft->y;
	double llz = lowleft->z;

	point3d_t *upright = cuboid->upright;
	double urx = upright->x;
	double ury = upright->y;
	double urz = upright->z;

	fprintf(stream,
"Separator {\n\
	Coordinate3 {\n\
		point [\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
			%lf %lf %lf,\n\
		]\n\
	}\n\
\n\
	IndexedLineSet {\n\
		coordIndex [\n\
			0, 1, 2, 0, -1,\n\
			0, 2, 3, 0, -1,\n\
			7, 6, 5, 7, -1,\n\
			7, 5, 4, 7, -1,\n\
			0, 3, 7, 0, -1,\n\
			0, 7, 4, 0, -1,\n\
			1, 5, 6, 1, -1,\n\
			1, 6, 2, 1, -1,\n\
			0, 4, 5, 0, -1,\n\
			0, 5, 1, 0, -1,\n\
			3, 2, 6, 3, -1,\n\
			3, 6, 7, 3, -1\n\
		]\n\
	}\n\
}\n",
	urx, ury, urz,
	llx, ury, urz,
	llx, lly, urz,
	urx, lly, urz,
	urx, ury, llz,
	llx, ury, llz,
	llx, lly, llz,
	urx, lly, llz);
}

status_t cuboid_initialize_matrices(matrix_t **matrices, double *ll, double *ur)
{
	status_t error = SUCCESS;

	double urx = ur[0], ury = ur[1], urz = ur[2];
	double llx = ll[0], lly = ll[1], llz = ll[2];

	INITIALIZE_OR_OUT_OF_MEM //upper right point
	(
		matrices[0],
		matrix_initialize_with_array(4, 1, (double[]) { urx, ury, urz, 1 }),
		error, error0
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		matrices[1],
		matrix_initialize_with_array(4, 1, (double[]) { llx, ury, urz, 1 }),
		error, error1
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		matrices[2],
		matrix_initialize_with_array(4, 1, (double[]) { llx, lly, urz, 1 }),
		error, error2
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		matrices[3],
		matrix_initialize_with_array(4, 1, (double[]) { urx, lly, urz, 1 }),
		error, error3
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		matrices[4],
		matrix_initialize_with_array(4, 1, (double[]) { urx, ury, llz, 1 }),
		error, error4
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		matrices[5],
		matrix_initialize_with_array(4, 1, (double[]) { llx, ury, llz, 1 }),
		error, error5
	);

	INITIALIZE_OR_OUT_OF_MEM //lower left point
	(
		matrices[6],
		matrix_initialize_with_array(4, 1, (double[]) { llx, lly, llz, 1 }),
		error, error6
	);

	INITIALIZE_OR_OUT_OF_MEM
	(
		matrices[7],
		matrix_initialize_with_array(4, 1, (double[]) { urx, lly, llz, 1 }),
		error, error7
	);

	goto success;

error7:
	matrix_uninitialize(matrices[6]);
error6:
	matrix_uninitialize(matrices[5]);
error5:
	matrix_uninitialize(matrices[4]);
error4:
	matrix_uninitialize(matrices[3]);
error3:
	matrix_uninitialize(matrices[2]);
error2:
	matrix_uninitialize(matrices[1]);
error1:
	matrix_uninitialize(matrices[0]);
error0:

success:
	return error;
}

void cuboid_uninitialize_matrices(matrix_t **matrices)
{
	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_uninitialize(matrices[i]);
	}
}

void cuboid_print_matrices_to_iv(matrix_t **matrices, FILE *stream)
{
	fprintf(stream,
"Separator {\n\
	Coordinate3 {\n\
		point [\n");

	for (size_t i = 0; i < CUBOID_POINTS; i++)
	{
		matrix_t *m = matrices[i];
		fprintf(stream,
"			%lf %lf %lf,\n", matrix_get(m, 0, 0), matrix_get(m, 1, 0), matrix_get(m, 2, 0));
	}

	fprintf(stream,
"		]\n\
	}\n\
\n\
	IndexedLineSet {\n\
		coordIndex [\n\
			0, 1, 2, 0, -1,\n\
			0, 2, 3, 0, -1,\n\
			7, 6, 5, 7, -1,\n\
			7, 5, 4, 7, -1,\n\
			0, 3, 7, 0, -1,\n\
			0, 7, 4, 0, -1,\n\
			1, 5, 6, 1, -1,\n\
			1, 6, 2, 1, -1,\n\
			0, 4, 5, 0, -1,\n\
			0, 5, 1, 0, -1,\n\
			3, 2, 6, 3, -1,\n\
			3, 6, 7, 3, -1\n\
		]\n\
	}\n\
}\n");
}
