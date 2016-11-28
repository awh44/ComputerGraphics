#include <stdlib.h>

#include "cuboid.h"
#include "point3d.h"

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
