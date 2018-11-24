#ifndef RENDERER_OBJECT_MESH_H_
#define RENDERER_OBJECT_MESH_H_

#include "core/math/BoundingBox.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class RrCModel;
class rrMeshBuffer;

namespace renderer
{
	//	class Mesh
	// A Mesh provides a mechanism to draw a single rrMesh.
	class Mesh : public CRenderableObject
	{
	public:
		RENDER_API explicit		Mesh ( rrMeshBuffer*, bool n_enableSkinning=false );
		RENDER_API virtual		~Mesh ( void );

	public:
		//	GetName ()
		// Return my mesh's name
		RENDER_API const char* const
								GetName ( void ) const
			{ return m_name.c_str(); }

		//	GetBoundingBox ()
		// Gets the bounding box of the mesh
		RENDER_API core::math::BoundingBox
								GetBoundingBox ( void ) const
			{ return bbCheckRenderBox; }

		//	GetCanRender ()
		// Get if frustum culling hides this mesh
		RENDER_API const bool	GetCanRender ( void ) const
			{ return bCanRender; }

		//		PreRender
		// Frustum culling & material Cbuffer update.
		RENDER_API bool			PreRender ( rrCameraPass* cameraPass ) override;

		//		Render
		// Called during engine render pass.
		RENDER_API bool			Render ( const rrRenderParams* params ) override;

	protected:
		// Get the mesh bounding box
		void					CalculateBoundingBox ( void );

	public:
		rrMeshBuffer*		m_mesh;
		RrCModel*			m_parent;

	protected:
		friend RrCModel;
		bool				bUseFrustumCulling;
		bool				bCanRender;
		bool				bUseSkinning;

		// Frustum Culling
		Vector3d			vCheckRenderPos;
		float				fCheckRenderDist;
		Vector3d			vMinExtents;
		Vector3d			vMaxExtents;
		core::math::BoundingBox
							bbCheckRenderBox;

		arstring64			m_name;
	};
}

#endif//RENDERER_OBJECT_MESH_H_