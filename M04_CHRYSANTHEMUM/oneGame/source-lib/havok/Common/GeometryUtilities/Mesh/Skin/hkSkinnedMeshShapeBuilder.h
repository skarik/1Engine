/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SKINNED_MESH_SHAPE_BUILDER_H
#define HK_SKINNED_MESH_SHAPE_BUILDER_H

#include <Common/GeometryUtilities/Mesh/Skin/hkSkinnedMeshShape.h>
#include <Common/GeometryUtilities/Mesh/hkMeshSystem.h>
#include <Common/GeometryUtilities/Mesh/hkMeshVertexBuffer.h>
#include <Common/GeometryUtilities/Mesh/Utils/PrimitiveUtil/hkMeshPrimitiveUtil.h>

//
//	Skinned mesh builder

class hkSkinnedMeshBuilder
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkSkinnedMeshBuilder);

	public:

			/// Constructor
		hkSkinnedMeshBuilder(hkSkinnedMeshShape* skinnedMeshShape, hkMeshSystem* meshSystem, int maxBonesPerMesh);

			/// Destructor
		~hkSkinnedMeshBuilder();

	public:

			/// Adds a mesh
		void addMesh(const hkMeshShape* meshShape, const hkQTransform& tm);

			/// Builds the skinned mesh shape
		void build();

	protected:

			/// Descriptor for a mesh section
		struct MeshSection
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkSkinnedMeshBuilder::MeshSection);

				/// Compares two sections by surface index
			HK_FORCE_INLINE int compareTo(const MeshSection& other) const;

				/// Index of the mesh in m_sourceMeshes
			int m_meshIndex;

				/// Index of the surface in m_surfaces
			int m_surfaceIndex;

				/// Index of the sub-mesh
			int m_subMeshIndex;

				/// Original start vertex
			int m_originalStartVertex;

				/// Original number of vertices
			int m_originalNumVertices;
		};

			/// Descriptor for a skin
		struct SkinDescriptor
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkSkinnedMeshBuilder::SkinDescriptor);

			SkinDescriptor()
			:	m_startBoneIndex(0)
			,	m_numBones(0)
			{}

				/// Sorts all sections by material Id
			void sortSections();

				/// Counts the number of sub-meshes
			int countSubmeshes() const;

				/// Index of the first bone in this skin
			int m_startBoneIndex;

				/// Number of bones in this skin
			int m_numBones;

				/// Mesh sections in this skin
			hkArray<MeshSection> m_sections;
		};

	protected:

			/// Adds a new part
		void addPart(hkUint16 boneIdx, hkUint16 meshSectionIndex, int startVtx, int numVertices, int startIdx, int numIndices, const hkAabb& aabb);

			/// Adds a surface. Returns its index
		int addSurface(hkMeshMaterial* surface);

			/// Computes the shared vertex buffer format
		void computeVertexFormat();

			/// Computes the number of vertices in the given skin
		int computeNumVertices(SkinDescriptor& sd);

			/// Fills the provided skinned vertex buffer with the data from the given skin descriptor
		void fillSkinnedVertexBuffer(hkMeshVertexBuffer* skinnedVb, SkinDescriptor& sd);

			/// Copies a sub-set of vertices from one locked vertex buffer to another
		void copyVertices(	const hkMeshVertexBuffer::LockedVertices& dstVerts, const hkVertexFormat& dstVtxFmt, int dstStartVertex, 
							const hkMeshVertexBuffer::LockedVertices& srcVerts, const hkVertexFormat& srcVtxFmt, int srcStartVertex, int numVerts);

			/// Set-up the bone weights & indices
		void createBoneWeightsAndIndices(hkMeshVertexBuffer* skinnedVb, SkinDescriptor& sd);

			/// Compute common index buffer format
		void computeIndexFormat(SkinDescriptor& sd, hkMergeMeshPrimitvesCalculator& mpc);

			/// Creates the mesh for the given descriptor. The indices are not properly set at this stage
		hkMeshShape* createMesh(const SkinDescriptor& sd, const hkMergeMeshPrimitvesCalculator& mpc, hkMeshVertexBuffer* vb);

			/// Fills the provided skinned index buffer with the data from the given skin descriptor
		void fillSkinnedIndexBuffer(SkinDescriptor& sd, hkMeshShape* skinnedMesh);

			/// Computes the render vertex range
		void calcRenderVertexRange(const hkMeshSection& meshSection, int& startVertexIndex, int& numVertices);

			/// Applies the given transform to the given range of vertices
		void applyTransform(const hkQTransform& mtx, const hkMeshVertexBuffer::LockedVertices& verts, int startVertex, int numVertices);

			/// Computes the AABB of the given sub-set of vertices in the given vertex buffer
		void calcAabb(const hkMeshVertexBuffer::LockedVertices& verts, int startVertex, int numVertices, hkAabb& aabbOut);

			/// Returns the current skin descriptor
		HK_FORCE_INLINE SkinDescriptor* getCurrentSkinDescriptor();

	protected:

			/// Maximum number of bones per mesh
		int m_maxNumBonesPerMesh;
		
			/// Array of unique materials
		hkArray<hkMeshMaterial*> m_surfaces;

			/// Array of source meshes
		hkArray<const hkMeshShape*> m_sourceMeshes;

			/// Array of source mesh transforms
		hkArray<hkQTransform> m_sourceMeshTransforms;

			/// Array of existing skins
		hkArray<SkinDescriptor> m_skinDescriptors;

			/// Vertex buffer format
		hkVertexFormat m_vfmt;

			/// The resulting skinned mesh shape
		hkSkinnedMeshShape* m_skinnedMeshShape;

			/// The mesh system
		hkMeshSystem* m_meshSystem;
};

#endif	//	HK_SKINNED_MESH_SHAPE_BUILDER_H

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
