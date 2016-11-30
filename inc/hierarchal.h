#ifndef _HIERARCHAL_H_
#define _HIERARCHAL_H_

#include "matrix.h"
#include "status.h"

typedef struct
{
	matrix_t **matrices;
} model_t;

typedef struct hierarchal_t
{
	model_t model;
	matrix_t *from_parent;
	void (*draw)(model_t *, matrix_t *transform);
	struct hierarchal_t *sibling;
	struct hierarchal_t *child;
} hierarchal_t;

status_t hierarchal_draw(hierarchal_t *model, matrix_t *transform);

#endif
