/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_MESH_PRIMITIVE_UTIL_H
#define HK_MESH_PRIMITIVE_UTIL_H

#include <Common/GeometryUtilities/Mesh/hkMeshShape.h>

	/// Small helper utility to work on triangle indices
class hkMeshPrimitiveUtil
{
    public:
        enum PrimitiveStyle
        {
            PRIMITIVE_STYLE_UNKNOWN = 0,
            PRIMITIVE_STYLE_POINT,
            PRIMITIVE_STYLE_LINE,
            PRIMITIVE_STYLE_TRIANGLE,
        };

            /// Returns the amount of indices for a primitive type and a number of primitives
        static int HK_CALL calculateNumIndices(hkMeshSection::PrimitiveType type, int numPrims);

            /// Returns the amount of primitives for a primitive type and an amount of indices
        static int HK_CALL calculateNumPrimitives(hkMeshSection::PrimitiveType type, int numIndices);

			/// Append triangle indices from non indexed primitives
		static void appendTriangleIndices(hkMeshSection::PrimitiveType primType, int numVertices, int indexBase, hkArray<hkUint16>& indicesOut)
		{
			appendTriangleIndices(primType, numVertices, indexBase, indicesOut, hkContainerHeapAllocator().get(&indicesOut));
		}
		static void HK_CALL appendTriangleIndices(hkMeshSection::PrimitiveType primType, int numVertices, int indexBase, hkArrayBase<hkUint16>& indicesOut, hkMemoryAllocator& a);

			/// Append triangle indices from non indexed primitives
		static void HK_CALL appendTriangleIndices(hkMeshSection::PrimitiveType primType, int numVertices, int indexBase, hkArray<hkUint32>& indicesOut);

			/// Append triangle indices from indexed primitives
        static void HK_CALL appendTriangleIndices16(hkMeshSection::PrimitiveType primType, const hkUint16* indices, int numIndices, int indexBase, hkArray<hkUint16>& indicesOut);

			/// Append triangle indices from indexed primitives
		static void HK_CALL appendTriangleIndices32(hkMeshSection::PrimitiveType primType, const hkUint32* indices, int numIndices, int indexBase, hkArray<hkUint32>& indicesOut);

			/// Returns a section as list of triangle indices - must have been locked with indices
		static void HK_CALL appendTriangleIndices(const hkMeshSection& section, hkArray<hkUint16>& indicesOut);

			/// Returns a section as list of triangle indices - must have been locked with indices
		static void HK_CALL appendTriangleIndices(const hkMeshSection& section, hkArray<hkUint32>& indicesOut);

            /// Returns a section as list of triangle indices
        static void HK_CALL appendTriangleIndices(const hkMeshShape* shape, int sectionIndex, hkArray<hkUint16>& indicesOut);

            /// Get the primitive style type
        static PrimitiveStyle HK_CALL getPrimitiveStyle(hkMeshSection::PrimitiveType type);

			/// Copies the indices from source to destination, with conversion
		static void HK_CALL copyPrimitives(	const void* srcIndexPtr, hkMeshSection::PrimitiveType srcPrimitiveType, hkMeshSection::MeshSectionIndexType srcIndexType, int numPrimitives,
											void* dstIndexPtr, hkMeshSection::PrimitiveType dstPrimitiveType, hkMeshSection::MeshSectionIndexType dstIndexType, int baseIndexOffset);

			/// Returns the size in bytes of a single index entry
		static HK_FORCE_INLINE int HK_CALL getIndexEntrySize(hkMeshSection::MeshSectionIndexType idxType) { return (idxType << 1); }
};

	/// Utility class to determine the common (most general) mesh primitive type
class hkMergeMeshPrimitvesCalculator
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkMergeMeshPrimitvesCalculator);

	public:

			/// Constructor
		hkMergeMeshPrimitvesCalculator();

			/// Adds a set of primitives
		void add(int numPrimitives, hkMeshSection::PrimitiveType primtiveType, hkMeshSection::MeshSectionIndexType indexType);

			/// Returns the primitive type
		HK_FORCE_INLINE hkMeshSection::PrimitiveType getPrimitiveType() const		{ return m_mergedPrimitiveType; }

			/// Returns the number of primitives
		HK_FORCE_INLINE int getNumPrimitives() const								{ return m_numTotalPrimitives; }

			/// Returns the number of indices
		HK_FORCE_INLINE int getNumIndices() const									{ return hkMeshPrimitiveUtil::calculateNumIndices(getPrimitiveType(), getNumPrimitives()); }

			/// Returns the index type
		HK_FORCE_INLINE hkMeshSection::MeshSectionIndexType getIndexType() const	{  return (getNumIndices() > 0xFFFF) ? hkMeshSection::INDEX_TYPE_UINT32 : m_mergedIndexType; }

	protected:

			/// Merged primitive type
		hkMeshSection::PrimitiveType m_mergedPrimitiveType;

			/// Merged index type
		hkMeshSection::MeshSectionIndexType m_mergedIndexType;

			/// Total number of primitives
		int m_numTotalPrimitives;
};

#endif // HK_MESH_PRIMITIVE_UTIL_H

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
