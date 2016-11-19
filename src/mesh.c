#include "mesh.h"

#include "mesh_face_vec.h"
#include "point3d_vec.h"
#include "status.h"

#define MALLOC_OR_GOTO(obj, errvar, label)\
	do\
	{\
		if ((obj = malloc(sizeof *obj)) == NULL)\
		{\
			errvar = OUT_OF_MEM;\
			goto label;\
		}\
	} while (0)

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

/*
static status_t add_faces_to_vec
(
	mesh_face_vec_t *faces,
	size_t curr_row_curr_col,
	size_t curr_row_next_col,
	size_t next_row_curr_col,
	size_t next_row_next_col
)
{
	status_t error = SUCCESS;

	mesh_face_t *new_face1;
	mesh_face_t *new_face2;
	if (((new_face1 = malloc(sizeof *new_face1)) == NULL) ||
		((new_face2 = malloc(sizeof *new_face2)) == NULL))
	{
		//Must be either NULL (safe to free) or allocated
		free(new_face1);
		error = OUT_OF_MEM;
		goto exit0;
	}

	new_face1->vertices[0] = curr_row_curr_col;
	new_face1->vertices[1] = curr_row_next_col;
	new_face1->vertices[2] = next_row_curr_col;

	new_face2->vertices[0] = curr_row_next_col;
	new_face2->vertices[1] = next_row_next_col;
	new_face2->vertices[2] = next_row_curr_col;

	mesh_face_vec_push_back(faces, new_face1);
	mesh_face_vec_push_back(faces, new_face2);

exit0:
	return error;
}
*/

static status_t add_faces_new
(
	mesh_face_vec_t *faces,
	size_t curr_row_curr_col,
	size_t curr_row_next_col,
	size_t next_row_curr_col,
	size_t next_row_next_col
)
{
	status_t error = SUCCESS;

	mesh_face_t *new_face1;
	mesh_face_t *new_face2;
	if (((new_face1 = malloc(sizeof *new_face1)) == NULL) ||
		((new_face2 = malloc(sizeof *new_face2)) == NULL))
	{
		//Must be either NULL (safe to free) or allocated
		free(new_face1);
		error = OUT_OF_MEM;
		goto exit0;
	}

	new_face1->vertices[0] = next_row_curr_col;
	new_face1->vertices[1] = next_row_next_col;
	new_face1->vertices[2] = curr_row_next_col;

	new_face2->vertices[0] = next_row_curr_col;
	new_face2->vertices[1] = curr_row_next_col;
	new_face2->vertices[2] = curr_row_curr_col;

	mesh_face_vec_push_back(faces, new_face1);
	mesh_face_vec_push_back(faces, new_face2);

exit0:
	return error;
}

static status_t add_faces(mesh_face_vec_t *faces, size_t row, size_t col, size_t num_per_row)
{
	size_t curr_row_curr_col = row * num_per_row + col;
	size_t curr_row_next_col = curr_row_curr_col + 1;
	size_t next_row_curr_col = curr_row_curr_col + num_per_row;
	size_t next_row_next_col = curr_row_next_col + num_per_row;
	return add_faces_new(faces, curr_row_curr_col, curr_row_next_col, next_row_curr_col, next_row_next_col);
}

status_t mesh_calculate_faces(mesh_t *mesh)
{
	status_t error = SUCCESS;
	mesh_face_vec_t *faces = mesh->faces;
	size_t num_u = mesh->num_u;
	size_t num_v = mesh->num_v;

	size_t i;
	for (i = 0; i < num_u - 1; i++)
	{
		size_t j;
		for (j = 0; j < num_v - 1; j++)
		{
			if ((error = add_faces(faces, i, j, num_v)))
			{
				goto exit0;
			}
		}
	}

exit0:
	return error;
}

static status_t add_first_fan(mesh_face_vec_t *faces, size_t num_per_row)
{
	status_t error = SUCCESS;
	mesh_face_t *new_face;

	size_t i;
	for (i = 0; i < num_per_row - 1; i++)
	{
		MALLOC_OR_GOTO(new_face, error, exit0);
		new_face->vertices[0] = i + 1;
		new_face->vertices[1] = i + 2;
		new_face->vertices[2] = 0;
		mesh_face_vec_push_back(faces, new_face);
	}

	MALLOC_OR_GOTO(new_face, error, exit0);
	new_face->vertices[0] = i + 1;
	new_face->vertices[1] = 1;
	new_face->vertices[2] = 0;
	mesh_face_vec_push_back(faces, new_face);

exit0:
	return error;
}

static status_t add_last_fan(mesh_face_vec_t *faces, size_t num_per_row, size_t num_points)
{
	status_t error = SUCCESS;
	mesh_face_t *new_face;

	size_t last_index = num_points - 1;

	size_t i;
	for (i = 0; i < num_per_row - 1; i++)
	{
		MALLOC_OR_GOTO(new_face, error, exit0);
		new_face->vertices[0] = last_index;
		new_face->vertices[1] = last_index - (i + 1);
		new_face->vertices[2] = last_index - (i + 2);
		mesh_face_vec_push_back(faces, new_face);
	}

	MALLOC_OR_GOTO(new_face, error, exit0);
	new_face->vertices[0] = last_index;
	new_face->vertices[1] = last_index - (i + 1);
	new_face->vertices[2] = last_index - 1;
	mesh_face_vec_push_back(faces, new_face);

exit0:
	return error;
}

status_t mesh_calculate_sellipsoid_faces(mesh_t *mesh)
{
	status_t error = SUCCESS;
	mesh_face_vec_t *faces = mesh->faces;
	size_t num_v = mesh->num_v;
	size_t num_u = mesh->num_u;

	//Add a triangle fan from the first pole point to all the points in the first row.
	if ((error = add_first_fan(faces, num_u - 1)))
	{
		goto exit0;
	}

	/*
		Indices here are a little "funky" because of the lone poles at the ends.
		Not going to treat the pole as a "row", so start j at 0 for the first
		"row" of points and, when calculating the indices, just add 1 to account
		for the pole. For the stop condition: normally, we'd stop at the second-to-last
		row, row-index num_v - 2, but because the pole has to be treated differently, we
		would stop at row-index num_v - 3. However, because the indices are "off" a row
		because of the *first* pole, we need to stop at row-index num_v - 4.
	*/
	size_t j;
	for (j = 0; j < num_v - 3; j++)
	{
		/*
			Index is a little weird here again because the last point in the row has
			to connect back to the first one in the row, so normally, we'd have to
			stop before we processed num_u - 1. However, because the row actually only
			*has* num_u points, as mentioned some other places, we have to stop before
			processing num_u - 2 instead.
		 */
		size_t i;
		for (i = 0; i < num_u - 2; i++)
		{
			//i + 1 to account for the pole; num_u - 1 because each row actually only
			//has num_u - 1 points, because point num_u - 1 would be the same as point 0
			if ((error = add_faces(faces, j, i + 1, num_u - 1)))
			{
				goto exit0;
			}
		}

		//Process the "wraparound" point
		size_t curr_j_curr_i = j * (num_u - 1) + i + 1;
		size_t curr_j_frst_i = j * (num_u - 1) + 1;
		size_t next_j_curr_i = curr_j_curr_i + (num_u - 1);
		size_t next_j_frst_i = curr_j_frst_i + (num_u - 1);
		add_faces_new(faces, curr_j_curr_i, curr_j_frst_i, next_j_curr_i, next_j_frst_i);
	}

	//Add a triangle fan from all the points in the last row to the last pole point.
	if ((error = add_last_fan(faces, num_u - 1, point3d_vec_size(mesh->points))))
	{
		goto exit0;
	}

exit0:
	return error;
}

void mesh_print_to_iv(mesh_t *mesh, FILE *stream)
{
	fprintf(stream,
"Separator {\n\
	ShapeHints {\n\
		vertexOrdering	COUNTERCLOCKWISE\n\
	}\n\
\n\
  Coordinate3 {\n\
    point [\n");

	size_t i;

	point3d_vec_t *points = mesh->points;
	size_t num_points = point3d_vec_size(mesh->points);
	for (i = 0; i < num_points; i++)
	{
		point3d_t *point = point3d_vec_get(points, i);
		fprintf(stream,
"	%lf %lf %lf,\n", point->x, point->y, point->z);
	}

	fprintf(stream,
"    ]\n\
  }\n\
\n");

	size_t num_normals = point3d_vec_size(mesh->normals);
	if (num_normals > 0)
	{
		fprintf(stream,
"NormalBinding {\n\
  value        PER_VERTEX_INDEXED\n\
	}\n\
\n\
  Normal {\n\
    vector [\n");

		for (i = 0; i < num_normals; i++)
		{
			point3d_t *normal = point3d_vec_get(mesh->normals, i);
			fprintf(stream,
"	%lf %lf %lf,\n", normal->x, normal->y, normal->z);
		}

		fprintf(stream,
"    ]\n\
 }\n");

	}

	fprintf(stream,
"  IndexedFaceSet {\n\
    coordIndex [\n");

	mesh_face_vec_t *faces = mesh->faces;
	size_t num_faces = mesh_face_vec_size(faces);
	for (i = 0; i < num_faces; i++)
	{
		mesh_face_t *face = mesh_face_vec_get(faces, i);
		fprintf(stream,
	"	 %zu, %zu, %zu, -1,\n", face->vertices[0], face->vertices[1], face->vertices[2]);
	}
	fprintf(stream,
"    ]\n\
  }\n\
}\n");
}

