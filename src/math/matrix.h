/**************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <ostream>

#include "qmatrix.h"

/**
This class performs matrix storage, lu decomposition, forward and backward
substitution, and a few other useful operations. Steps in using class:
(1) Create an instance of this class with the correct size
(2) Define the matrix pattern as neccessary:
	(1) Call zero (unnecessary after initial ceration) to reset the pattern
		& matrix
	(2) Call setUse to set the use of each element in the matrix
	(3) Call createMap to generate the row-wise permutation mapping for use
		in partial pivoting
(3) Add the values to the matrix
(4) Call performLU, and get the results with fbSub
(5) Repeat 2, 3, 4 or 5 as necessary.
@short Matrix manipulation class tailored for circuit equations
@author David Saxton
*/
class Matrix
{
public:
	/**
	 * Creates a size x size square matrix m, with all values zero,
	 * and a right side vector x of size m+n
	 */
	Matrix(CUI size);
	~Matrix();

	/**
	 * Returns true if the matrix is changed since last calling performLU()
	 * - i.e. if we do need to call performLU again.
	 */
	inline bool isChanged() const { return max_k < m_mat->size_m(); }
	/**
	 * Performs LU decomposition. Going along the rows,
	 * the value of the decomposed LU matrix depends only on
	 * the previous values.
	 */
	void performLU();
	/**
	 * Applies the right side vector (x) to the decomposed matrix,
	 * with the solution returned in x.
	 */
	void fbSub(QuickVector *x);
	/**
	 * Prints the matrix to stdout
	 */
	void displayMatrix(std::ostream &outstream) const;
	/**
	 * Prints the LU-decomposed matrix to stdout
	 */
	void displayLU(std::ostream &outstream) const;

	void identity() { *m_mat = 1; max_k = 0; }

	double &g(CUI i, CUI j) {
		const unsigned int mapped_i = m_inMap[i];
		if(mapped_i < max_k) max_k = mapped_i;

		if(j < max_k) max_k = j;

		// I think I need the next line...
		if(max_k > 0) max_k--;

		return(*m_mat)[mapped_i][j];
	}

	double  g(CUI i, CUI j) const { return m_mat->at(m_inMap[i], j); }

	/**
	 * Multiplies this matrix by the Vector rhs, and places the result
	 * in the vector pointed to by result. Will fail if wrong size.
	 */
	void multiply(const QuickVector *rhs, QuickVector *result );
	double validateLU() const;

private:
	/**
	 * Swaps around the rows in the (a) the matrix; and (b) the mappings
	 */
	void swapRows(CUI a, CUI b);
	unsigned int max_k; // optimization variable, allows partial L_U re-do. 
	int *m_inMap; // Rowwise permutation mapping from external reference to internal storage

	QuickMatrix *m_mat;
	QuickMatrix *m_lu;
	double *m_y; // Avoids recreating it lots of times
};

#endif
