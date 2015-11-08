/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_TRANSFORM_H
#define HK_MATH_TRANSFORM_H

#ifndef HK_MATH_MATH_H
#	error Please include Common/Base/hkBase.h instead of this file.
#endif

/// Stores a rotation and translation.
/// Note that valid hkTransforms always have an orthonormal rotation component.
/// Note that the w component of the columns may not be set correctly. Havok
/// implicitly assumes that the shear components are all 0 and that the w
/// component of the translation is 1. To get a 4x4 matrix suitable for rendering
/// use the get4x4ColumnMajor method.
///
/// The following conventions are adopted:
///     - Internal representation of rotation is column-major
///     - Applying matrix multiplication to a vector is done by premultiplying
///       by the matrix
///
/// IMPORTANT NOTE ON SCALING:
///
/// This class is not designed to represent a full transformation including
/// skewing and scaling components. The assumption is that skewing will not be
/// used and that scaling is not meaningful in a simulation of rigid bodies
/// that only change their position and orientation.
///
/// Do not attempt to put scaling into the rows or columns of the
/// rotation matrix. It must remain orthogonal at all times.
class hkTransform
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkTransform);
		HK_DECLARE_POD_TYPE();

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
			/// Default constructor - all elements are uninitialized.
		HK_FORCE_INLINE hkTransform() { }

			/// Creates a new hkTransform using the rotation r and translation t.
		HK_FORCE_INLINE hkTransform(const hkRotation& r, hkVector4Parameter t);

			/// Copy constructor
		HK_FORCE_INLINE hkTransform(const hkTransform& r);

			/// Creates a new hkTransform using the rotation quaternion q and translation t.
		HK_FORCE_INLINE hkTransform(hkQuaternionParameter q, hkVector4Parameter t);
#endif

			/// Read-write access element at (row, column)
		HK_FORCE_INLINE hkReal& operator() (int row, int column);

			/// Read-only access element at (row, column)
		HK_FORCE_INLINE const hkReal& operator() (int row, int column) const;

		template <int ROW, int COL>
		HK_FORCE_INLINE const hkSimdReal getElement() const;

		template <int ROW, int COL>
		HK_FORCE_INLINE void setElement(hkSimdRealParameter s);

			/// Sets the hkTransform's values using the rotation quaternion q and translation t.
		HK_FORCE_INLINE void set(hkQuaternionParameter q, hkVector4Parameter t);
		HK_FORCE_INLINE void set(const hkRotation& r, hkVector4Parameter t);

			/// Sets this hkTransform to be the identity transform.
		HK_FORCE_INLINE void setIdentity();

			/// Returns a global identity transform.
		HK_FORCE_INLINE static const hkTransform& HK_CALL getIdentity();

			/// Checks if this transform is the identity within an optional epsilon
		bool isApproximatelyEqual( const hkTransform& t, hkReal epsilon=hkReal(1e-3f) ) const;

			/// Checks if this transform is the identity within an epsilon
		bool isApproximatelyEqualSimd( const hkTransform& t, hkSimdRealParameter epsilon ) const;

			/// Gets a writable translation component.
		HK_FORCE_INLINE hkVector4& getTranslation();

			/// Gets the translation component.
		HK_FORCE_INLINE const hkVector4& getTranslation() const;

			/// Sets the translation component.
		HK_FORCE_INLINE void setTranslation(hkVector4Parameter t);

			/// Gets a writable rotation component.
		HK_FORCE_INLINE hkRotation& getRotation();

			/// Gets the rotation component.
		HK_FORCE_INLINE const hkRotation& getRotation() const;

			/// Sets the rotation component (using a hkRotation).
		HK_FORCE_INLINE void setRotation(const hkRotation& rotation);

			/// Sets the rotation component (using a hkQuaternion).
		HK_FORCE_INLINE void setRotation(hkQuaternionParameter quatRotation);

			/// Sets this transform to be the inverse of the given transform t.
		void setInverse( const hkTransform &t );

			/// Sets this transform to be the inverse of the given transform t.
		HK_FORCE_INLINE void _setInverse( const hkTransform &t );

			/// Sets this transform to be the product of t1 and t2. (this = t1 * t2)
		void setMul( const hkTransform &t1, const hkTransform &t2 );

			/// Sets this transform to be the product of t1 and t2. (this = t1 * t2)
		void setMul( const hkQsTransform &t1, const hkTransform &t2 );

			/// Sets this transform to be the product of the inverse of t1 by t2. (this = t1^-1 * t2)
		void setMulInverseMul( const hkTransform& bTa, const hkTransform &bTc );

			/// Sets this transform to be the product of the inverse of t1 by t2. (this = t1^-1 * t2)
		HK_FORCE_INLINE void _setMulInverseMul( const hkTransform& bTa, const hkTransform &bTc );

			/// Sets this transform to be the product of itself and the transform b. (this *= b)
		void setMulEq( const hkTransform& b );

			/// Writes a 4x4 matrix to \a p in column-major order. 
			/// The pointer \a p must be suitably aligned, depending on the math lib build.
			/// See the notes for aligned store in hkVector4.
		void get4x4ColumnMajor(hkFloat32* HK_RESTRICT p) const;
			/// Writes a 4x4 matrix suitable for rendering into \a p. The pointer \a p must be 32-Byte aligned.
		void get4x4ColumnMajor(hkDouble64* HK_RESTRICT p) const;

			/// Reads a 4x4 matrix from \a p in column-major order. The matrix is only valid if there is no scaling present and the rotation is orthonormal.
			/// The pointer \a p must be suitably aligned, depending on the math lib build.
			/// See the notes for aligned load in hkVector4.
		void set4x4ColumnMajor(const hkFloat32* p);
			/// Reads a 4x4 matrix from \a p. There should be no scaling and the rotation should be orthonormal. The pointer \a p must be 32-Byte aligned.
		void set4x4ColumnMajor(const hkDouble64* p);

			/// Sets all 4 rows at once (including the .w component)
		HK_FORCE_INLINE void setRows4( hkVector4Parameter r0, hkVector4Parameter r1, hkVector4Parameter r2, hkVector4Parameter r3);


			/// Sets this transform to be the product of a and the inverse of b. (this = a * b^-1)
		void setMulMulInverse( const hkTransform &a, const hkTransform &b );

			/// Checks for bad values (denormals or infinities).
		bool isOk() const;

			/// Gets a read-write reference to the i'th column.
		HK_FORCE_INLINE hkVector4& getColumn(int i);

			/// Gets a read-only reference to the i'th column.
		HK_FORCE_INLINE const hkVector4& getColumn(int i) const;

		template <int I>
		HK_FORCE_INLINE const hkVector4& getColumn() const;

		template <int I>
		HK_FORCE_INLINE void setColumn(hkVector4Parameter v);

	protected:

		hkRotation m_rotation;
		hkVector4 m_translation;
};

#endif //HK_MATH_TRANSFORM_H

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
