#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdio.h>

//If I make this into more of a "real" library, I should create a "matrix
//status" enum instead. This would give a way to do bounds checking as
//well
#include "status.h"

typedef struct matrix_t matrix_t;

/*
 * matrix_initialize - initializes a new matrix with the given rows and columns,
 * with all values as zeros
 * @param rows - the number of rows in the matrix
 * @param cols - the number of columns in the matrix
 * @return - the new matrix
 */
matrix_t *matrix_initialize(size_t rows, size_t cols);

/*
 * matrix_initialize_with_array - initializes a new matrix with the given rows
 * and columns, initialized with the values specified by the array treated as
 * being in row-major form. Note that no bounds checking can be done on the
 * array, to it must be at least as large as rows * cols
 * @param rows - the number of rows in the matrix
 * @param cols - the number of columns in the matrix
 * @return - the new matrix
 */
matrix_t *matrix_initialize_with_array(size_t rows, size_t cols, double *array);

/*
 * matrix_copy - creates a new copy of the given matrix
 * @param orig - the matrix to copy
 * @return - the copy of the original matrix
 */
matrix_t *matrix_copy(matrix_t *orig);

/*
 * matrix_uninitialize - uninitializes the given matrix
 * @param m - the matrix to uninitialize
 */
void matrix_uninitialize(matrix_t *m);

/*
 * matrix_assign_from_array - given an array treated in row-major form, assigns
 * it to the given matrix. Note that no bounds checking can be done, so the
 * array must be as large as the number of rows and columns in the matrix.
 * @param m     - the matrix to which to assign
 * @param array - the array to assign to the matrix
 */
void matrix_assign_from_array(matrix_t *m, double *array);

/*
 * matrix_assign - assigns the values in the source matrix to the destination
 * matrix. Note that no bounds checking is done so note that the matrices must
 * match in size.
 * @param dst - the destination matrix in which to store the values
 * @param src - the source matrix from which to get the values
 */
void matrix_assign(matrix_t *dst, matrix_t *src);

/*
 * matirx_zero - zeros the matrix out
 * @param m - the matrix to zero
 */
void matrix_zero(matrix_t *m);

/*
 * matrix_rows - retrieves the number of rows in the given matrix.
 * @param m - the matrix of which to get the number of rows
 * @return - the number of rows in the matrix
 */
size_t matrix_rows(matrix_t *m);

/*
 * matrix_cols - retrieves the number of columns in the given matrix.
 * @param m - the matrix of which to get the number of columns
 * @return - the number of columns in the matrix
 */
size_t matrix_cols(matrix_t *m);

/*
 * matrix_get - retrieves the value at the given row and column in the matrix.
 * Note that no bounds checking is done, so the row and column must reside
 * within the matrix.
 * @param m   - the matrix from which to retrieve the value
 * @param row - the row from which to retrieve the value
 * @param col - the column from which to retrieve the value
 */
double matrix_get(matrix_t *m, size_t row, size_t col);

/*
 * matrix_set - sets the value at the given row and column in the matrix. Note
 * that no bounds checking is done, so the row and column must reside within
 * the matrix.
 * @param m   - the matrix in which to set the value
 * @param row - the row in which to set the value
 * @param col - the column in which to set the value
 */
void matrix_set(matrix_t *m, size_t row, size_t col, double val);

/*
 * matrix_multiply - performs a matrix multiplication between a and b and
 * stores the value in c, i.e., c = ab. Note that no bounds checking is done,
 * so the number of columns in a must match the number of rows in b, and the
 * number of rows in c must match the number of rows in a and the number of 
 * columns in c must match the number of columns in b. Also note that c cannot
 * alias either a or b.
 * @param c - the matrix in which to store the result of the multiplication
 * @param a - the left-hand matrix in the multiplication
 * @param b - the right-hand matri in the multiplication
 */
void matrix_multiply(matrix_t *c, matrix_t *a, matrix_t *b);

/*
 * matrix_multiply_alis - performs a matrix multiplication between a and b and
 * stores the value in c, i.e., c = ab. Note that no bounds checking is done,
 * so the number of columns in a must match the number of rows in b, and the
 * number of rows in c must match the number of rows in a and the number of
 * columns in c must match the number of columns in b. The only difference
 * between this function and matrix_multiply is that this one allows cm to
 * alias am and bm
 * @param c - the matrix in which to store the result of the multiplication
 * @param a - the left-hand matrix in the multiplication
 * @param b - the right-hand matri in the multiplication
 * @return - an indication of whether the function failed or not
 */
status_t matrix_multiply_alias(matrix_t *cm, matrix_t *am, matrix_t *bm);

/*
 * matrix_print - performs a very simple print of the matrix to the given stream
 * @param m      - the matrix to print
 * @param stream - the stream to which to print
 */
void matrix_print(matrix_t *m, FILE *stream);

#endif
