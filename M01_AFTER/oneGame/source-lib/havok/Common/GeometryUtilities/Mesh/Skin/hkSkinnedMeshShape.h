/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SKINNED_MESH_SHAPE_H
#define HK_SKINNED_MESH_SHAPE_H

#include <Common/GeometryUtilities/Mesh/hkMeshShape.h>

extern const hkClass hkSkinnedMeshShapeClass;
extern const hkClass hkStorageSkinnedMeshShapeClass;

	/// Abstract skinned mesh shape. It is a collection of bone sections, where a bone section is driven by a contiguous range of bones.
	/// Each bone section has a collection of parts, where a part represents a contiguous portion of the vertex and index buffers, rendered
	/// with the same material. 
class hkSkinnedMeshShape : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA);
		HK_DECLARE_REFLECTION();

	public:

			/// Bone section deforming a hkMeshShape
		struct BoneSection
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkSkinnedMeshShape::BoneSection);
			HK_DECLARE_REFLECTION();

				/// Constructor
			BoneSection();

				/// Serialization constructor
			BoneSection(class hkFinishLoadedObjectFlag flag);

				/// The mesh buffer
			hkRefPtr<hkMeshShape> m_meshBuffer;

				/// Start bone
			hkInt16 m_startBoneIndex;

				/// Number of bones
			hkInt16 m_numBones;
		};

			/// Vertex / index buffer part. Driven by a single bone, with a single material, has a contiguous section of the vertex and index buffer.
		struct Part
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkSkinnedMeshShape::Part);
			HK_DECLARE_REFLECTION();

				/// Start vertex
			int m_startVertex;

				/// Number of vertices
			int m_numVertices;

				/// Start index
			int m_startIndex;

				/// Number of indices
			int m_numIndices;

				/// Bone index
			hkUint16 m_boneIndex;

				/// Mesh section index
			hkUint16 m_meshSectionIndex;

				/// Bounding sphere 
			hkVector4 m_boundingSphere;
		};

	public:

			/// Constructor
		hkSkinnedMeshShape();

			/// Serialization constructor
		hkSkinnedMeshShape(class hkFinishLoadedObjectFlag flag);

			/// Destructor
		virtual ~hkSkinnedMeshShape();

	public:

			/// Gets the number of bone sections
		virtual int getNumBoneSections() const = 0;

			/// Returns the bone section at the given index
		virtual void getBoneSection(int boneSectionIndex, BoneSection& boneSectionOut) const = 0;

			/// Returns the number of parts
		virtual int getNumParts() const = 0;

			/// Returns the part at the given index
		virtual void getPart(int partIndex, Part& partOut) const = 0;

			/// Prints a lot of debug info about the mesh
		virtual void dbgOut() const;

			/// Gets the name of this shape
		virtual const char* getName() const { return HK_NULL; }

			/// Sets the name of this shape
		virtual void setName(const char* name) {}

		//
		//	Builder interface
		//

	public:

			/// Adds a new bone section
		virtual void addBoneSection(hkMeshShape* meshShape, int startBoneIdx, int numBones) {}

			/// Adds a new part
		virtual void addPart(const Part& p) {}

			/// Sorts the parts by increasing bone indices
		virtual void sortParts() {}
};

	/// Skinned mesh shape which stores the sections and parts in internal arrays
class hkStorageSkinnedMeshShape : public hkSkinnedMeshShape
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA);
		HK_DECLARE_REFLECTION();

	public:

			/// Constructor
		hkStorageSkinnedMeshShape();

			/// Serialization constructor
		hkStorageSkinnedMeshShape(class hkFinishLoadedObjectFlag flag);

	public:

			/// Sets the mesh shape on the given bone section
		HK_FORCE_INLINE void setBoneSectionMeshShape(int idx, hkMeshShape* newShape) { m_boneSections[idx].m_meshBuffer = newShape; }

	public:

			/// Gets the number of bone sections
		virtual int getNumBoneSections() const;

			/// Returns the bone section at the given index
		virtual void getBoneSection(int boneSectionIndex, BoneSection& boneSectionOut) const;

			/// Returns the number of parts
		virtual int getNumParts() const;

			/// Returns the part at the given index
		virtual void getPart(int partIndex, Part& partOut) const;

			/// Sorts the parts by increasing bone indices
		virtual void sortParts();

			/// Adds a new bone section
		virtual void addBoneSection(hkMeshShape* meshShape, int startBoneIdx, int numBones);

			/// Adds a new part
		virtual void addPart(const Part& p);

			/// Returns the class type
		virtual const hkClass* getClassType() const;

			/// Gets the name of this shape
		virtual const char* getName() const;

			/// Sets the name of this shape
		virtual void setName(const char* name);

	protected:

			/// Bone sections
		hkArray<BoneSection> m_boneSections;

			/// Parts
		hkArray<Part> m_parts;

			/// Name
		hkStringPtr m_name;
};

#endif	//	HK_SKINNED_MESH_SHAPE_H

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
