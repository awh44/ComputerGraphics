#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

struct matrix_t
{
	double *elems;
	size_t rows;
	size_t cols;
};

#define ELEMENT(m, row, col) m->elems[row * m->cols + col]

matrix_t *matrix_initialize(size_t rows, size_t cols)
{
	matrix_t *m;
	if ((m = malloc(sizeof *m)) == NULL)
	{
		goto error0;
	}

	//Technically, filling a double with zeros doesn't guarantee the double
	//value 0.0, but it will in most situations
	if ((m->elems = calloc(rows * cols, sizeof *m->elems)) == NULL)
	{
		goto error1;
	}

	m->rows = rows;
	m->cols = cols;

	goto success;

error1:
	free(m);
	m = NULL;
error0:

success:
	return m;
}

static matrix_t *matrix_init_uninit(size_t rows, size_t cols)
{
	matrix_t *m;
	if ((m = malloc(sizeof *m)) == NULL)
	{
		goto error0;
	}

	if ((m->elems = malloc(rows * cols * sizeof *m->elems)) == NULL)
	{
		goto error1;
	}

	m->rows = rows;
	m->cols = cols;

	goto success;

error1:
	free(m);
	m = NULL;
error0:

success:
	return m;
}

matrix_t *matrix_initialize_with_array(size_t rows, size_t cols, double *array)
{
	matrix_t *m;
	if ((m = matrix_init_uninit(rows, cols)) == NULL)
	{
		goto error0;
	}
	memcpy(m->elems, array, rows * cols * sizeof *m->elems);

	goto success;

error0:

success:
	return m;

}

matrix_t *matrix_copy(matrix_t *orig)
{
	return matrix_initialize_with_array(orig->rows, orig->cols, orig->elems);
}

void matrix_uninitialize(matrix_t *m)
{
	free(m->elems);
	free(m);
}

void matrix_assign_from_array(matrix_t *m, double *array)
{
	memcpy(m->elems, array, m->rows * m->cols * sizeof *m->elems);
}

void matrix_assign(matrix_t *dst, matrix_t *src)
{
	memcpy(dst->elems, src->elems, dst->rows * dst->cols * sizeof *dst->elems);
}

static inline void array_zero(double *a, size_t size)
{
	//This doesn't guarantee the actual value 0.0, but it's fine for my purposes
	memset(a, 0, size * sizeof *a);
}

void matrix_zero(matrix_t *m)
{
	array_zero(m->elems, m->rows * m->cols);
}

size_t matrix_rows(matrix_t *m)
{
	return m->rows;
}

size_t matrix_cols(matrix_t *m)
{
	return m->cols;
}

double matrix_get(matrix_t *m, size_t row, size_t col)
{
	return ELEMENT(m, row, col);
}

void matrix_set(matrix_t *m, size_t row, size_t col, double val)
{
	ELEMENT(m, row, col) = val;
}

static void matrix_multiply_internal(double *c, double *a, double *b, size_t arows, size_t acols, size_t bcols)
{
	//I'd need to do some benchmarking to see whether memset'ing and using this
	//loop structure or using a "regular" i, j, k loop structure and not having
	//to memset is better.
	array_zero(c, arows * bcols);
	for (size_t i = 0; i < arows; i++)
	{
		for (size_t k = 0; k < acols; k++)
		{
			for (size_t j = 0; j < bcols; j++)
			{
				c[i * bcols + j] += a[i * acols + k] * b[k * bcols + j];
			}
		}
	}
}

void matrix_multiply(matrix_t *cm, matrix_t *am, matrix_t *bm)
{
	matrix_multiply_internal(cm->elems, am->elems, bm->elems, am->rows, am->cols, bm->cols);
}

status_t matrix_multiply_alias(matrix_t *cm, matrix_t *am, matrix_t *bm)
{
	status_t error = SUCCESS;

	double *cnew;
	INITIALIZE_OR_OUT_OF_MEM(cnew, calloc(am->rows * bm->cols, sizeof *cnew), error, exit0);

	matrix_multiply_internal(cnew, am->elems, bm->elems, am->rows, am->cols, bm->cols);
	memcpy(cm->elems, cnew, cm->rows * cm->cols * sizeof *cm->elems);

	free(cnew);
exit0:
	return error;
}

void matrix_print(matrix_t *m, FILE *stream)
{
	size_t i;
	for (i = 0; i < m->rows; i++)
	{
		fprintf(stream, "[");
		size_t j;
		for (j = 0; j < m->cols; j++)
		{
			fprintf(stream, " %lf", ELEMENT(m, i, j));
		}

		fprintf(stream, " ]\n");
	}
}
