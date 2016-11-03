#include "mesh.h"

#include "mesh_face_vec.h"
#include "point3d_vec.h"

mesh_t *mesh_initialize(void)
{
	mesh_t *mesh;
	if ((mesh = malloc(sizeof *mesh)) == NULL)
	{
		goto error0;
	}

	if ((mesh->points = point3d_vec_initialize()) == NULL)
	{
		goto error1;
	}

	if ((mesh->faces = mesh_face_vec_initialize()) == NULL)
	{
		goto error2;
	}

	goto success;

error2:
	point3d_vec_uninitialize(mesh->points);
error1:
	free(mesh);
	mesh = NULL;
error0:

success:
	return mesh;
}

void mesh_uninitialize(mesh_t *mesh)
{
	point3d_vec_uninitialize(mesh->points);
	mesh_face_vec_uninitialize(mesh->faces);
	free(mesh);
}

void mesh_print_to_iv(mesh_t *mesh, FILE *stream)
{
	point3d_vec_t *points = mesh->points;
	size_t i;
	for (i = 0; i < point3d_vec_size(points); i++)
	{
		point3d_print_to_iv(point3d_vec_get(points, i), stream, 0.1);
	}
}

