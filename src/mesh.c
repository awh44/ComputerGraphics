#include "mesh.h"

#include "mesh_face_vec.h"
#include "point3d_vec.h"
#include "status.h"

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

	if ((mesh->normals = point3d_vec_initialize()) == NULL)
	{
		goto error3;
	}

	goto success;

error3:
	mesh_face_vec_uninitialize(mesh->faces);
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
	size_t i;

	point3d_vec_t *points = mesh->points;
	size_t num_points = point3d_vec_size(points);
	for (i = 0; i < num_points; i++)
	{
		point3d_uninitialize(point3d_vec_get(points, i));
	}
	point3d_vec_uninitialize(points);

	mesh_face_vec_t *faces = mesh->faces;
	size_t num_faces = mesh_face_vec_size(faces);
	for (i = 0; i < num_faces; i++)
	{
		free(mesh_face_vec_get(faces, i));
	}
	mesh_face_vec_uninitialize(faces);

	point3d_vec_t *normals = mesh->normals;
	size_t num_normals = point3d_vec_size(normals);
	for (i = 0; i < num_normals; i++)
	{
		point3d_uninitialize(point3d_vec_get(normals, i));
	}
	point3d_vec_uninitialize(normals);


	free(mesh);
}

status_t mesh_calculate_faces(mesh_t *mesh)
{
	status_t error = SUCCESS;

	size_t num_u = mesh->num_u;
	size_t num_v = mesh->num_v;

	size_t i;
	for (i = 0; i < num_u - 1; i++)
	{
		size_t j;
		for (j = 0; j < num_v - 1; j++)
		{
			size_t curr_i_curr_j = i * num_u + j;
			size_t curr_i_next_j = i * num_u + (j + 1);
			size_t next_i_curr_j = (i + 1) * num_u + j;
			size_t next_i_next_j = (i + 1) * num_u + (j + 1);

			mesh_face_t *new_face1;
			mesh_face_t *new_face2;
			if (((new_face1 = malloc(sizeof *new_face1)) == NULL) ||
				((new_face2 = malloc(sizeof *new_face2)) == NULL))
			{
				error = OUT_OF_MEM;
				goto exit0;
			}

			new_face1->vertices[0] = curr_i_curr_j;
			new_face1->vertices[1] = curr_i_next_j;
			new_face1->vertices[2] = next_i_curr_j;

			new_face2->vertices[0] = curr_i_next_j;
			new_face2->vertices[1] = next_i_next_j;
			new_face2->vertices[2] = next_i_curr_j;

			mesh_face_vec_push_back(mesh->faces, new_face1);
			mesh_face_vec_push_back(mesh->faces, new_face2);
		}
	}

exit0:
	return error;
}

void mesh_print_to_iv(mesh_t *mesh, FILE *stream)
{
	fprintf(stream,
"Separator {\n\
	Coordinate3 {\n\
		point [\n");

	size_t i;

	point3d_vec_t *points = mesh->points;
	size_t num_points = point3d_vec_size(mesh->points);
	for (i = 0; i < num_points; i++)
	{
		point3d_t *point = point3d_vec_get(points, i);
		fprintf(stream,
"			%lf %lf %lf,\n", point->x, point->y, point->z);
	}

	size_t num_normals = point3d_vec_size(mesh->normals);
	if (num_normals > 0)
	{
		fprintf(stream,
"		]\n\
	}\n\
\
	NormalBinding {\n\
		value	PER_VERTEX_INDEXED\n\
	}\n\
\n\
	Normal {\n\
		vector [\n");

		for (i = 0; i < num_normals; i++)
		{
			point3d_t *normal = point3d_vec_get(mesh->normals, i);
			fprintf(stream,
"			%lf %lf %lf,\n", normal->x, normal->y, normal->z);
		}

		fprintf(stream,
"		]\n\
	}\n");

	}

	fprintf(stream,
"	IndexedFaceSet {\n\
		coordIndex [\n");

	mesh_face_vec_t *faces = mesh->faces;
	size_t num_faces = mesh_face_vec_size(faces);
	for (i = 0; i < num_faces; i++)
	{
		mesh_face_t *face = mesh_face_vec_get(faces, i);
		fprintf(stream,
	"			%zu %zu %zu -1,\n", face->vertices[0], face->vertices[1], face->vertices[2]);
	}
	fprintf(stream,
"		]\n\
	}\n\
}\n");
}

