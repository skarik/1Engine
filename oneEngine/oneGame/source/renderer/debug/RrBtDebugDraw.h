#ifndef RENDERER_DEBUG_RR_BT_DEBUG_DRAW_H_
#define RENDERER_DEBUG_RR_BT_DEBUG_DRAW_H_

#include "core/types/ModelData.h"
#include "renderer/object/CRenderableObject.h"
#include "bullet/LinearMath/btIDebugDraw.h"

class PrWorld;

class RrBtDebugDraw : public btIDebugDraw, public CRenderableObject
{
public:
	RENDER_API explicit		RrBtDebugDraw ( PrWorld* associated_world );
	RENDER_API				~RrBtDebugDraw ( void );

	//		PreRender()
	// Push the model's uniform up up.
	RENDER_API bool			PreRender ( void ) override;

	//		Render()
	// Render the model using the 2D engine's style
	RENDER_API bool			Render ( const char pass ) override;

	//		PostRender()
	// Push the current model information to the GPU.
	RENDER_API bool			EndRender ( void ) override;

public:
	// Bullet interface:

	RENDER_API void			drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;

	RENDER_API void			drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override;

	RENDER_API void			reportErrorWarning(const char* warningString) override {}

	RENDER_API void			draw3dText(const btVector3& location,const char* textString) override {}

	RENDER_API void			setDebugMode(int debugMode) override;

	RENDER_API int			getDebugMode() const override;

private:
	// Associated world
	PrWorld*	m_world;

	// Debug rendering mode
	int			m_debugMode;

private:
	//	Mesh information
	std::vector<arModelVertex>	m_vertexData;
	std::vector<uint32_t>		m_indexData;
	uint32_t				m_gpuIndexCount;
	bool					m_haveNewUpload;

	//	GPU information (todo: abstract this)
	uint					m_buffer_verts;
	uint					m_buffer_tris;
};

#endif//RENDERER_DEBUG_RR_BT_DEBUG_DRAW_H_