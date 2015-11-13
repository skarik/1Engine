/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_MATH_GEOMETRY_H
#define HK_MATH_GEOMETRY_H


extern const class hkClass hkGeometryTriangleClass;

extern const hkClass hkGeometryClass;

///	Stores a triangle based mesh.
struct hkGeometry
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkGeometry);
	HK_DECLARE_REFLECTION();

		/// A hkTriangle contains indices into an array of vertices and a material index.
	struct Triangle
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE_CLASS, hkGeometry::Triangle );
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

		void set( int a, int b, int c, int material = -1){	m_a = a; m_b = b; m_c = c;	m_material = material;	}

		int m_a;
		int m_b;
		int m_c;
		int m_material;
	};


		/// Constructor
	hkGeometry ()	{	}

		/// Copy constructor. Required since hkArray's copy constructor is not public.
	hkGeometry (const hkGeometry& other);

		/// Finish constructor (for internal use).
	hkGeometry (hkFinishLoadedObjectFlag f);

		/// Clear content.
	void	clear();

		/// Check if data is numerically valid
	hkBool isValid() const;

		/// Returns a read-only reference to a triangle vertex.
	const hkVector4& getVertex(int triangleIndex, int vertexIndex) const { HK_ASSERT2(0xB9C6C634, vertexIndex >= 0 && vertexIndex < 3, "Index out-of-range"); return m_vertices[(&m_triangles[triangleIndex].m_a)[vertexIndex]]; }

		/// Returns a read-write reference to a triangle vertex.
	hkVector4& getVertex(int triangleIndex, int vertexIndex) { HK_ASSERT2(0xB9C6C634, vertexIndex >= 0 && vertexIndex < 3, "Index out-of-range"); return m_vertices[(&m_triangles[triangleIndex].m_a)[vertexIndex]]; }

		/// Copies the triangle vertices in a preallocated buffer passed as parameter
	HK_FORCE_INLINE void getTriangle(int triangleIndex, hkVector4* HK_RESTRICT vertices) const
	{ 
		HK_ASSERT2(0xB9C6C634, triangleIndex >= 0 && triangleIndex < m_triangles.getSize(), "Index out-of-range"); 
		vertices[0] = m_vertices[m_triangles[triangleIndex].m_a];
		vertices[1] = m_vertices[m_triangles[triangleIndex].m_b];
		vertices[2] = m_vertices[m_triangles[triangleIndex].m_c];
	}

		/// Append another geometry (vertex welding not performed).
	void appendGeometry(const hkGeometry& geometry, const hkTransform& transform = hkTransform::getIdentity());



        //
        // Fields:
        //

		/// Array of vertices that the triangles can index into.
	hkArray<hkVector4> m_vertices;

		/// Array of triangles. The triangles are quadruples of ints with the first 3 being indices into the m_vertices array.
	hkArray<struct Triangle> m_triangles;
};

#endif // HK_MATH_GEOMETRY_H

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
