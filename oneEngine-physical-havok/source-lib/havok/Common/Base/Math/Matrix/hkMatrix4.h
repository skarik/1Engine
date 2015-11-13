/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_MATRIX4_H
#define HK_MATH_MATRIX4_H

#ifndef HK_MATH_MATH_H
#	error Please include Common/Base/hkBase.h instead of this file.
#endif

namespace hkCompileError
{
	template <bool b> struct HK_MATRIX4_FUNC_NOT_IMPLEMENTED;
	template <> struct HK_MATRIX4_FUNC_NOT_IMPLEMENTED<true>{ };
}
#define HK_MATRIX4_NOT_IMPLEMENTED HK_COMPILE_TIME_ASSERT2(N==0, HK_MATRIX4_FUNC_NOT_IMPLEMENTED)

/// A 4x4 matrix of hkReals, representing a transformation.
/// This transform representation is used by tools, cloth and display libraries, as
/// it can represent scale and skew.
/// Elements are stored in column major format,
/// i.e., contiguous memory locations are (x00, x10, x20, x30), (x01, x11,...)
/// where x10 means row 1, column 0 for example.
/// For orthonormal transforms, use hkTransform instead (storage is equivalent).
/// Some operations in hkMatrix4 are optimized for affine matrices (those where
/// the last row is 0,0,0,1).
class hkMatrix4
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMatrix4);
		HK_DECLARE_POD_TYPE();

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
			/// Empty constructor. The elements of the matrix are not initialized.
		HK_FORCE_INLINE hkMatrix4() { } 
#endif

			/// Gets a read-only reference to the i'th column.
		HK_FORCE_INLINE const hkVector4& getColumn(int i) const;

			/// Gets a read-write reference to the i'th column.
		HK_FORCE_INLINE hkVector4& getColumn(int i);

		HK_FORCE_INLINE void setColumn(int i, hkVector4Parameter c);

		template <int I>
		HK_FORCE_INLINE const hkVector4& getColumn() const;

		template <int I>
		HK_FORCE_INLINE void setColumn(hkVector4Parameter c);

			/// Returns a copy of the i'th row.
		HK_FORCE_INLINE void getRow( int row, hkVector4& r) const;

		template <int I>
		HK_FORCE_INLINE void getRow( hkVector4& r ) const;

		HK_FORCE_INLINE void setRow( int row, hkVector4Parameter r );

		template <int I>
		HK_FORCE_INLINE void setRow( hkVector4Parameter r );

			/// Gets read-write access to the specified element.
		HK_FORCE_INLINE hkReal& operator() (int row, int col);

			/// Gets read-only access to the specified elements.
		HK_FORCE_INLINE const hkReal& operator() (int row, int col) const;

		template <int ROW, int COL>
		HK_FORCE_INLINE const hkSimdReal getElement() const;

		template <int ROW, int COL>
		HK_FORCE_INLINE void setElement(hkSimdRealParameter s);

			/// Sets all rows at once.
		HK_FORCE_INLINE void setRows( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2, hkVector4Parameter r3);

			/// Writes the rows 0 to 3 in to the parameters r0, r1, r2, r3 respectively.
		HK_FORCE_INLINE void getRows( hkVector4& r0, hkVector4& r1, hkVector4& r2, hkVector4& r3) const;

			/// Sets all columns of the current matrix. Where column is set to r0 and so on.
		HK_FORCE_INLINE void setCols( hkVector4Parameter c0, hkVector4Parameter c1, hkVector4Parameter c2, hkVector4Parameter c3);

			/// Writes the columns 0 to 3 into the parameters c0, c1, c2 and c3 respectively.
		HK_FORCE_INLINE void getCols( hkVector4& c0, hkVector4& c1, hkVector4& c2, hkVector4& c3) const;

			/// Zeroes all values in this matrix.
		HK_FORCE_INLINE void setZero();

			/// Sets the specified diagonal values, zeroes the non-diagonal values.
		HK_FORCE_INLINE void setDiagonal( hkReal m00, hkReal m11, hkReal m22, hkReal m33 = hkReal(1) );

		HK_FORCE_INLINE void setDiagonal( hkSimdRealParameter m00, hkSimdRealParameter m11, hkSimdRealParameter m22, hkSimdRealParameter m33 );

		HK_FORCE_INLINE void setDiagonal( hkVector4Parameter v );

			/// Sets the diagonal values to 1, zeroes the non-diagonal values.
		HK_FORCE_INLINE void setIdentity();

			/// Returns a global identity transform.
		HK_FORCE_INLINE static const hkMatrix4& HK_CALL getIdentity();

			/// Set the contents based on the given hkTransform. Will set the bottom row to (0,0,0,1) in this hkMatrix4 as
			/// it is undefined in a hkTransform (not used)
		HK_FORCE_INLINE void set (const hkTransform& t);

			/// Set the contents based on the given hkTransform. Will set the bottom row to (0,0,0,1) in this hkMatrix4 as
			/// it is undefined in a hkTransform (not used)
		void set (const hkQTransform& qt);

			/// Configure the given transform with the content of self. As the transform is missing the bottom matrix row,
			/// it is simply discarded. Converting back and forth will thus not give the same matrix.
		HK_FORCE_INLINE void get (hkTransform& t) const;

			/// Writes a 4x4 matrix to \a p in column-major order. 
			/// The pointer \a p must be suitably aligned, depending on the math lib build.
			/// See the notes for aligned store in hkVector4.
		void get4x4ColumnMajor(hkFloat32* HK_RESTRICT p) const;
		void get4x4ColumnMajor(hkDouble64* HK_RESTRICT p) const;

			/// Reads a 4x4 matrix from \a p in column-major order.
			/// The pointer \a p must be suitably aligned, depending on the math lib build.
			/// See the notes for aligned load in hkVector4.
		void set4x4ColumnMajor(const hkFloat32* p);
		void set4x4ColumnMajor(const hkDouble64* p);

			/// Writes a 4x4 matrix to \a p in row-major order. 
			/// The pointer \a p must be suitably aligned, depending on the math lib build.
			/// See the notes for aligned store in hkVector4.
		void get4x4RowMajor(hkFloat32* HK_RESTRICT p) const;
		void get4x4RowMajor(hkDouble64* HK_RESTRICT p) const;

			/// Reads a 4x4 matrix from \a p in row-major order.
			/// The pointer \a p must be suitably aligned, depending on the math lib build.
			/// See the notes for aligned load in hkVector4.
		void set4x4RowMajor(const hkFloat32* p);
		void set4x4RowMajor(const hkDouble64* p);

			/// Checks if this matrix is equal to m within an optional epsilon.
		HK_FORCE_INLINE bool isApproximatelyEqual ( const hkMatrix4& m, hkReal epsilon = hkReal(1e-3f) ) const;

			/// Inverts the matrix. This function returns HK_SUCCESS if the determinant is greater than epsilon. Otherwise it returns HK_FAILURE and the matrix values are undefined.
		hkResult invert (hkReal epsilon);

			/// Sets the matrix to be the inverse of the given matrix. It returns HK_SUCCESS if the determinant is greater than epsilon. Otherwise it returns HK_FAILURE and the matrix values are undefined.
		hkResult setInverse (const hkMatrix4& m, hkReal epsilon);

			/// Transposes this matrix in place.
		void transpose();

			/// set to the transpose of another matrix
		HK_FORCE_INLINE void setTranspose( const hkMatrix4& s );
		
			/// (Assumes transforms are affine) Set this matrix to be the product of a and b. (this = a * b)
		HK_FORCE_INLINE void setMulAffine ( const hkMatrix4& a, const hkMatrix4& b );

			/// (Non-affine version - slower) Set this matrix to be the product of a and b. (this = a * b)
		HK_FORCE_INLINE void setMul ( const hkMatrix4& a, const hkMatrix4& b );

			/// (Assumes transforms are affine) Sets this matrix to be the product of a and the inverse of b. (this = a * b^-1)
		void setMulInverseAffine (  const hkMatrix4& a, const hkMatrix4& b );

			/// (Non-affine version - slower) Sets this matrix to be the product of a and the inverse of b. (this = a * b^-1)
		void setMulInverse (  const hkMatrix4& a, const hkMatrix4& b );

			/// Sets this matrix to be the product of a and scale (this = a * scale)
		HK_FORCE_INLINE void setMul( hkSimdRealParameter scale, const hkMatrix4& a );

			/// Modifies this matrix by adding the matrix a to it. (this += a)
		void add ( const hkMatrix4& a );

			/// Modifies this matrix by adding the matrix a to it. (this += a)
			/// This method is enforced inline.
		HK_FORCE_INLINE void _add ( const hkMatrix4& a );

			/// Modifies this matrix by subtracting the matrix a from it. (this += a)
		void sub ( const hkMatrix4& a );

			/// (Assumes transforms are affine) Modifies this matrix by post multiplying it by the matrix a. (this = this*a)
		HK_FORCE_INLINE void mulAffine ( const hkMatrix4& a);

			/// (Non-affine version - slower) modifies this matrix by post multiplying it by the matrix a. (this = this*a)
		HK_FORCE_INLINE void mul ( const hkMatrix4& a);

			/// Modifies this matrix by multiplying by scale (this *= scale)
		HK_FORCE_INLINE void mul ( hkSimdRealParameter scale );

			/// Copies all elements from a into this matrix.
		HK_FORCE_INLINE void operator= ( const hkMatrix4& a );

			/// Checks for bad values (denormals or infinities)
		bool isOk() const;

			/// Checks whether the matrix represents an affine transformation (the 4th row is 0,0,0,1)
		hkBool32 isAffineTransformation() const;

			/// Forces the matrix to represent an affine transformation (resets the 4th row to 0,0,0,1)
		void resetFourthRow ();

			/// Transforms a position - implicitly sets positionIn.w to 1.0f. positionOut.w is undefined
		HK_FORCE_INLINE void transformPosition (hkVector4Parameter positionIn, hkVector4& positionOut) const;

			/// Transforms a direction - implicitly sets directionIn.w to 0.0f, the directionOut.w is undefined.
		HK_FORCE_INLINE void transformDirection (hkVector4Parameter directionIn, hkVector4& directionOut) const;

			/// Transforms a direction with the transposed of this - implicitly sets directionIn.w to 0.0f.
		HK_FORCE_INLINE void transformInverseDirection (hkVector4Parameter directionIn, hkVector4& directionOut) const;

			/// Multiplies a 4-element vector by this matrix4. Notice that the 4th component of the vector (w) is very relevant here.
			/// Use "transformPosition" or "transformDirection" to transform vectors representing positions or directions by matrices
			/// representing transformations.
		HK_FORCE_INLINE void multiplyVector (hkVector4Parameter vectorIn, hkVector4& resultOut) const;

	protected:

		hkVector4 m_col0;
		hkVector4 m_col1;
		hkVector4 m_col2;
		hkVector4 m_col3;
};

#endif // HK_MATH_MATRIX4_H

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
