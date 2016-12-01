#ifndef _HIERARCHAL_H_
#define _HIERARCHAL_H_

#include "matrix.h"
#include "status.h"

typedef struct
{
	matrix_t **matrices;
} model_t;

typedef struct hierarchical_t
{
	model_t model;
	matrix_t *from_parent;
	void (*draw)(model_t *, matrix_t *transform);
	struct hierarchical_t *sibling;
	struct hierarchical_t *child;
} hierarchical_t;

status_t hierarchical_draw(hierarchical_t *model, matrix_t *transform);

#endif
