#ifndef _MESH_H_
#define _MESH_H_

#include "mesh_face_vec.h"
#include "point3d_vec.h"
#include "status.h"

typedef struct mesh_face_t
{
	size_t vertices[3];
} mesh_face_t;

struct mesh_face_vec_t;
typedef struct
{
	point3d_vec_t *points;
	size_t num_u;
	size_t num_v;
	struct mesh_face_vec_t *faces;
	point3d_vec_t *normals;
} mesh_t;

mesh_t *mesh_initialize(void);
void mesh_uninitialize(mesh_t *mesh);
status_t mesh_calculate_faces(mesh_t *mesh);
status_t mesh_calculate_sellipsoid_faces(mesh_t *mesh);
void mesh_print_to_iv(mesh_t *mesh, FILE *stream);
#endif
