/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_MATH_FOUR_TRANSPOSED_VECTORS_H
#define HK_MATH_FOUR_TRANSPOSED_VECTORS_H

class hkcdVertex;

	/// Represents 4 3D vertices bundled together. The vertices are transposed, i.e. (xxxx, yyyy, zzzz)
class hkFourTransposedPoints
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_COLLIDE, hkFourTransposedPoints );
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

	public:

		hkFourTransposedPoints()
		{
			m_vertices[0].setZero();
			m_vertices[1].setZero();
			m_vertices[2].setZero();
		}

			/// Sets the vertex bundle to the given vertices
		HK_FORCE_INLINE void set(hkVector4Parameter vA, hkVector4Parameter vB, hkVector4Parameter vC, hkVector4Parameter vD);

			/// Extracts the vectors from the vertex bundle
		HK_FORCE_INLINE void extract(hkVector4& vA, hkVector4& vB, hkVector4& vC, hkVector4& vD) const;

			/// Extracts the vectors from the vertex bundle, with w components inserted from wABCD.
		HK_FORCE_INLINE void extractWithW(hkVector4Parameter wABCD, hkVector4& vA, hkVector4& vB, hkVector4& vC, hkVector4& vD) const;

			/// Extracts the vector at index from the bundle
		HK_FORCE_INLINE void extract(int index, hkVector4& vI) const;

			/// Sets the vertex bundle to the same vertex
		HK_FORCE_INLINE void setAll(hkVector4Parameter v);

			/// Sets this = v - a
		HK_FORCE_INLINE void setSub(const hkFourTransposedPoints& v, hkVector4Parameter a);

			/// Sets this = a - v
		HK_FORCE_INLINE void setSub(hkVector4Parameter a, const hkFourTransposedPoints& v);

			/// Sets this = v - a
		HK_FORCE_INLINE void setSub(const hkFourTransposedPoints& v, const hkFourTransposedPoints& a);

			/// Sets this = v + a
		HK_FORCE_INLINE void setAdd(const hkFourTransposedPoints& v, const hkFourTransposedPoints& a);

			/// Sets this = vi * a, i = 0..3
		HK_FORCE_INLINE void setMul(const hkFourTransposedPoints& v, hkVector4Parameter a);

			/// Sets this = v * a
		HK_FORCE_INLINE void setMul(const hkFourTransposedPoints& v, const hkFourTransposedPoints& a);

			/// Sets v0 = b*c0, v1 = b*c1, etc.
		HK_FORCE_INLINE void setOuterProduct(hkVector4Parameter b, hkVector4Parameter c);

			/// Sets v0 += b*c0, v1 = b*c1, etc.
		HK_FORCE_INLINE void addOuterProduct(hkVector4Parameter b, hkVector4Parameter c);

			/// Sets v0 += b*c0, v1 = b*c1, etc.
		HK_FORCE_INLINE void subOuterProduct(hkVector4Parameter b, hkVector4Parameter c);

			/// Returns the dot3 of this and a
		HK_FORCE_INLINE void dot3(hkVector4Parameter a, hkVector4& dotOut) const;

			/// Returns the dot3 of this and a
		HK_FORCE_INLINE void dot3(const hkFourTransposedPoints& a, hkVector4& dotOut) const;

			/// Sets this = v + u * a
		HK_FORCE_INLINE void setAddMul(const hkFourTransposedPoints& v, const hkFourTransposedPoints& u, hkVector4Parameter a);

			/// Sets this = v - u * a
		HK_FORCE_INLINE void setSubMul(const hkFourTransposedPoints& v, const hkFourTransposedPoints& u, hkVector4Parameter a);

			/// Sets this += ui * a, i = 0..3
		HK_FORCE_INLINE void addMul(const hkFourTransposedPoints& u, hkVector4Parameter a);

			/// Sets this *= a
		HK_FORCE_INLINE void mul(hkVector4Parameter a);

			/// Sets this = cross(n, v)
		HK_FORCE_INLINE void setCross(hkVector4Parameter n, const hkFourTransposedPoints& v);

			/// Flips the signs, i.e. v0 = sign(ax) * v0, ..., v3 = sign(aw) * v3
		HK_FORCE_INLINE void flipSigns(hkVector4Parameter a);

			/// Selects a set of vectors i.e. vi = mask(i) ? ai : bi, for i = 0..3
		HK_FORCE_INLINE void setSelect(hkVector4ComparisonParameter mask, const hkFourTransposedPoints& trueVecs, const hkFourTransposedPoints& falseVecs);

			/// Sets this = Transpose(m) * v
		void setTransformedInverseDir(const hkMatrix3& m, const hkFourTransposedPoints& v);

			/// Inlined. Sets this = Transpose(m) * v
		HK_FORCE_INLINE void _setTransformedInverseDir(const hkMatrix3& m, const hkFourTransposedPoints& v);

			/// Sets this = Transpose(Rotation(m)) * (v - Translation(m))
		void setTransformedInversePos(const hkTransform& m, const hkFourTransposedPoints& v);

			/// Inlined. Sets this = Transpose(Rotation(m)) * (v - Translation(m))
		HK_FORCE_INLINE void _setTransformedInversePos(const hkTransform& m, const hkFourTransposedPoints& v);

			/// Sets this = m * v
		void setRotatedDir(const hkMatrix3& m, const hkFourTransposedPoints& v);

			/// Inlined. Sets this = m * v
		HK_FORCE_INLINE void _setRotatedDir(const hkMatrix3& m, const hkFourTransposedPoints& v);
		
			/// Normalizes the points
		HK_FORCE_INLINE void normalize();

			/// Copies the points from a hkVector4 array into a hkFourTransposedPoints array
		static void HK_CALL copyVertexData(const hkVector4* HK_RESTRICT sourceVerts, const int numVerts, hkArray<hkFourTransposedPoints>& pointsOut);

			/// The output array must have space for a multiple of 4 vertices = (numVertices|3) & ~3.
		static void getOriginalVertices( const hkFourTransposedPoints* verts4in, int numVertices, hkcdVertex* verticesOut );
		
	public:

			/// The vertices
		hkVector4 m_vertices[3];
};

#include <Common/Base/Math/Vector/hkFourTransposedPoints.inl>

#endif	//	HK_MATH_FOUR_TRANSPOSED_VECTORS_H

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
