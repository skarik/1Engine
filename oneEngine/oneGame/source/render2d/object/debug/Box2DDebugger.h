
#ifndef _RENDER2D_BOX2D_DEBUGGER_
#define _RENDER2D_BOX2D_DEBUGGER_

#include "core/types/ModelData.h"
#include "renderer/object/CRenderableObject.h"
#include "Box2D/Common/b2Draw.h"

class Box2DDebugger : public b2Draw, public CRenderableObject
{
public:
	RENDER2D_API explicit	Box2DDebugger ( void );
	RENDER2D_API virtual	~Box2DDebugger ();

	//		PostRender()
	// Push the current model information to the GPU.
	RENDER2D_API bool		EndRender ( void ) override;

	//		Render()
	// Render the model using the 2D engine's style
	RENDER2D_API bool		Render ( const char pass ) override;

public:
	// Box2D Interface:

	/// Draw a closed polygon provided in CCW order.
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a solid closed polygon provided in CCW order.
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a circle.
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	/// Draw a solid circle.
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	/// Draw a line segment.
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	void DrawTransform(const b2Transform& xf);

private:

	//	Mesh information
	arModelData				m_modeldata;
	//	GPU information
	uint					m_buffer_verts;
	uint					m_buffer_tris;

};

#endif//_RENDER2D_BOX2D_DEBUGGER_