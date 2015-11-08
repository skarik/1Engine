/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_MATRIX3_H
#define HK_MATH_MATRIX3_H

#ifndef HK_MATH_MATH_H
#	error Please include Common/Base/hkBase.h instead of this file.
#endif

namespace hkCompileError
{
	template <bool b> struct HK_MATRIX3_FUNC_NOT_IMPLEMENTED;
	template <> struct HK_MATRIX3_FUNC_NOT_IMPLEMENTED<true>{ };
}
#define HK_MATRIX3_NOT_IMPLEMENTED HK_COMPILE_TIME_ASSERT2(N==0, HK_MATRIX3_FUNC_NOT_IMPLEMENTED)

#if defined(HK_TINY_SPU_ELF)
#define HK_MATRIX3_INLINE HK_ALWAYS_INLINE
#else
#define HK_MATRIX3_INLINE HK_FORCE_INLINE
#endif

/// A 3x3 matrix of hkReals.
/// Due to alignment issues internal storage is 12 hkReals in a 4x3 matrix.
/// Elements are stored in column major format.
///
/// i.e., contiguous memory locations are (x00, x10, x20, pad), (x01, x11,...)
/// where x10 means row 1, column 0 for example.
class hkMatrix3
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMatrix3);
		HK_DECLARE_POD_TYPE();

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
			/// Empty constructor. The elements of the matrix are not initialized.
		HK_ALWAYS_INLINE hkMatrix3() {}

			/// Copy matrix from 'other'.
		HK_FORCE_INLINE hkMatrix3( const hkMatrix3& other );
#endif

			/// Gets a read-write reference to the i'th column.
		HK_FORCE_INLINE hkVector4& getColumn(int i);

			/// Gets a read-only reference to the i'th column.
		HK_FORCE_INLINE const hkVector4& getColumn(int i) const;
		template <int I> HK_MATRIX3_INLINE const hkVector4& getColumn() const;

		template <int I> HK_MATRIX3_INLINE void setColumn(hkVector4Parameter c);

			/// Gets a read-write reference to the i'th column.
		template <int I> HK_MATRIX3_INLINE hkVector4& getColumn();

			/// Returns a copy of the i'th row.
		HK_FORCE_INLINE void getRow( int row, hkVector4& r) const;
		template <int I> HK_FORCE_INLINE void getRow(hkVector4& r) const;

			/// Gets read-write access to the specified element.
		HK_FORCE_INLINE hkReal& operator() (int row, int col);

			/// Gets read-only access to the specified elements.
		HK_FORCE_INLINE const hkReal& operator() (int row, int col) const;

		template <int ROW, int COL> HK_FORCE_INLINE const hkSimdReal getElement() const;
		template <int ROW, int COL> HK_FORCE_INLINE void setElement(hkSimdRealParameter s);

			/// Sets all rows at once.
		HK_FORCE_INLINE void setRows( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2);

			/// Writes the rows 0 to 2 in to the parameters r0, r1 and r2 respectively.
		HK_FORCE_INLINE void getRows( hkVector4& r0, hkVector4& r1, hkVector4& r2) const;

			/// Sets all columns of the current matrix. Where column is set to r0 and so on.
		HK_FORCE_INLINE void setCols( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2);

			/// Writes the columns 0 to 2 into the parameters c0, c1 and c2 respectively.
		HK_FORCE_INLINE void getCols( hkVector4& r0, hkVector4& r1, hkVector4& r2) const;

			/// Zeroes all values in this matrix.
		HK_FORCE_INLINE void setZero();

			/// Sets the specified diagonal values, zeroes the non-diagonal values.
		HK_FORCE_INLINE void setDiagonal( hkReal m00, hkReal m11, hkReal m22 );
		HK_FORCE_INLINE void setDiagonal( hkSimdRealParameter m00, hkSimdRealParameter m11, hkSimdRealParameter m22 );
		HK_FORCE_INLINE void setDiagonal(hkVector4Parameter vDiagonal);
		HK_FORCE_INLINE void setDiagonal(hkSimdRealParameter diag);

			/// Sets the diagonal values to 1, zeroes the non-diagonal values.
		HK_FORCE_INLINE void setIdentity();

			/// Returns a global identity matrix3.
		HK_FORCE_INLINE static const hkMatrix3& HK_CALL getIdentity();

			/// Checks if this matrix is equal to m within an optional epsilon.
		bool isApproximatelyEqual( const hkMatrix3& m, hkReal epsilon=hkReal(1e-3f) ) const;

		/// Checks if this matrix is equal to m within an optional epsilon.
		bool isApproximatelyEqualSimd( const hkMatrix3& m, hkSimdRealParameter eps) const;
		
			/// Checks if this matrix is symmetric to within an optional epsilon.
		bool isSymmetric( hkReal epsilon=hkReal(1e-3f) ) const;

			/// Sets this to be r star (cross skew symmetric of vector r).
		void setCrossSkewSymmetric( hkVector4Parameter r );

			/// Inverts the matrix. This function returns HK_SUCCESS if the determinant is greater than epsilon^3. Otherwise it returns HK_FAILURE and the matrix values are undefined.
		hkResult invert(hkReal epsilon); 

			/// Sets the current matrix to be the inverse of the given matrix. Matrix src is assumed to be symmetric non-singular. If the matrix
			/// is singular (up to machine precision), the inverse will be set to zero.
		HK_FORCE_INLINE void setInverseSymmetric(const hkMatrix3& src);

			/// Solves the linear system: (this) * v = b, for v. The matrix is assumed non-singular and symmetric
		HK_FORCE_INLINE void solveSymmetric(hkVector4Parameter b, hkVector4& v);

			/// Inverts a symmetric matrix. This function assumes an invertible, symmetric, positive-definite matrix
		void invertSymmetric();

		/// Inverts a symmetric matrix. This function assumes an invertible, symmetric, positive-definite matrix
		HK_FORCE_INLINE void _invertSymmetric();

		//	///
		//void setRotatedDiagonalMatrix( const hkRotation &r, const hkVector4 &diagonalMatrix3 );

			/// Transposes this matrix in place.
		void transpose();

			/// set to the transpose of another matrix
		void setTranspose( const hkMatrix3& s );

			/// set to the transpose of another matrix
		HK_FORCE_INLINE void _setTranspose( const hkMatrix3& s );

			/// Set this matrix to be the product of a and b. (this = a * b)
		void setMul( const hkMatrix3& a, const hkMatrix3& b );

			/// Set this matrix to be the product of qa and b. (this = qa * b)
		HK_FORCE_INLINE void setMul(hkQuaternionParameter qa, const hkMatrix3& b);

			/// Set this matrix to be the product of a and b. (this = a * b)
		HK_FORCE_INLINE void _setMul( const hkMatrix3& a, const hkMatrix3& b );

			/// Sets this matrix to be the product of a and the inverse of b. (this = a * b^-1)
		void setMulInverse( const hkMatrix3& a, const hkRotation& b );

			/// Sets this matrix to be the product of the inverse of a and b. (this = a^-1 * b)
		void setMulInverseMul( const hkRotation& a, const hkMatrix3& b );

			/// Sets this matrix to be the product of a and scale. (this = a * scale)
		void setMul( hkSimdRealParameter scale, const hkMatrix3& a );

			/// Sets this matrix to be the product of a diagonal matrix (given as a vector) and matrix a. (this = diag * a)
		HK_FORCE_INLINE void setDiagonalMul(hkVector4Parameter vDiag, const hkMatrix3& a);

			/// Sets this matrix to be the product of a and scale. (this = a * scale)
		HK_FORCE_INLINE void _setMul( hkSimdRealParameter scale, const hkMatrix3& a );

			/// Sets this matrix to be the reflection of matrix D through matrix P, i.e., this = Transpose(P) * A * P.
			/// Matrix D is assumed diagonal.
		HK_FORCE_INLINE void setReflectedDiagonal(const hkMatrix3& P, hkVector4Parameter D);

			/// Adds the same value to all elements of the diagonal
		HK_FORCE_INLINE void addDiagonal(hkSimdRealParameter diag);

			/// Sets this = Transpose(a) * b.
		void setTransposeMul(const hkMatrix3& a, const hkMatrix3& b);

			/// Add the product of a and scale. (this += a * scale)
		void addMul( hkSimdRealParameter scale, const hkMatrix3& a );

			/// Add the product of a and scale. (this += a * scale)
		HK_FORCE_INLINE void _addMul( hkSimdRealParameter scale, const hkMatrix3& a );

			/// Rotates this matrix by the rotation matrix t. (this = t * this * t^-1)
		void changeBasis(const hkRotation& t);

			/// Modifies this matrix by adding the matrix a to it. (this += a)
		void add( const hkMatrix3& a );

			/// Modifies this matrix by adding the matrix a to it. (this += a)
		HK_FORCE_INLINE void _add( const hkMatrix3& a );

			/// Sets this matrix as the cross product between the given vector and each of the given matrix's columns.
			/// Specifically, it sets this.getColumn(i) = cross(v, src.getColumn(i)
		HK_FORCE_INLINE void setCross(hkVector4Parameter v, const hkMatrix3& src);

			/// Modifies this matrix by subtracting the matrix a from it. (this += a)
		void sub( const hkMatrix3& a );

			/// Modifies this matrix by subtracting the matrix a from it. (this += a)
		HK_FORCE_INLINE void _sub( const hkMatrix3& a );

			/// Modifies this matrix by post multiplying it by the matrix a. (this = this*a)
		void mul( const hkMatrix3& a);

			/// Modifies this matrix by multiplying by scale. (this *= scale)
		void mul( hkSimdRealParameter scale );

		/// Modifies this matrix by multiplying by scale. (this *= scale)
		HK_FORCE_INLINE void _mul( hkSimdRealParameter scale );

			/// Copies all elements from a into this matrix.
		HK_FORCE_INLINE void operator= ( const hkMatrix3& a );

			/// Checks for bad values (denormals or infinities)
		bool isOk() const;

			/// Multiplies a 3-element vector by this 3x3 matrix.
		HK_FORCE_INLINE void multiplyVector (hkVector4Parameter vectorIn, hkVector4& resultOut) const;

			/// Returns the determinant of this 3x3 matrix.
		const hkSimdReal getDeterminant() const;

			/// Returns the diagonal of this matrix.
		HK_FORCE_INLINE	void getDiagonal(hkVector4& diagonalOut) const;
					
			/// Diagonalizes a symmetric matrix, returning the eigenvectors in the columns of eigenVec (which is orthogonal) and the
			/// corresponding eigenvalues in eigenVal, with a specified tolerance epsilon and maximum number of Jacobi iterations maxIter.
			/// Returns HK_SUCCESS if the fractional off-diagonal norm was reduced below the specified epsilon, otherwise HK_FAILURE.
			/// You can interpret the eigenVec as a matrix which rotates points from eigenVal space to this space.
		hkResult diagonalizeSymmetric( hkRotation& eigenVec, hkVector4& eigenVal, int maxIter=20, hkReal epsilon=HK_REAL_EPSILON ) const;

			/// Diagonalizes a symmetric matrix using cached eigenvectors, returning the eigenvectors in the columns of eigenVec (which is orthogonal) and the
			/// corresponding eigenvalues in eigenVal, with a specified tolerance epsilon and maximum number of Jacobi iterations maxIter.
			/// Returns HK_SUCCESS if the fractional off-diagonal norm was reduced below the specified epsilon, otherwise HK_FAILURE.
		hkResult diagonalizeSymmetricWarmStart( hkMatrix3& eigenVec, hkVector4& eigenVal, int maxIter=20, hkReal epsilon=HK_REAL_EPSILON ) const;

			/// Like diagonalizeSymmetric() but without epsilon checks. Diagonalize by just iterating the max iter of times, no error checking.
		void diagonalizeSymmetricApproximation( hkRotation& eigenVec, hkVector4& eigenVal, int maxIter=10 ) const;
		

	protected:

		hkVector4 m_col0;
		hkVector4 m_col1;
		hkVector4 m_col2;
};

extern "C"
{
	// implementation of _setMul as out of line function with C-binding
	void hkMatrix3_setMulMat3Mat3( hkMatrix3* thisMatrix, const hkMatrix3& aTb, const hkMatrix3& bTc );
	void hkMatrix3_invertSymmetric( hkMatrix3& thisMatrix );
}

#endif //HK_MATH_MATRIX3_H

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120119)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
