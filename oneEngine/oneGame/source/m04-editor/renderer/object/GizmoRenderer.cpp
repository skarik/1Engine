#include "GizmoRenderer.h"

#include "m04-editor/entities/UIDragHandle.h"
#include "m04-editor/standalone/mapeditor/EditorObject.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glDrawing.h"

#include "core/math/Rect.h"
#include "core-ext/utils/MeshBuilder.h"

using namespace M04;

GizmoRenderer::GizmoRenderer ( void )
	: CStreamedRenderable3D()
{
	memset( &m_modeldata, 0, sizeof(arModelData) );

	// Set the default white material
	RrMaterial* defaultMat = new RrMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_MAIN, new RrTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( RrPassForward() );
	defaultMat->passinfo[0].shader = new RrShader( "shaders/sys/fullbright.glsl" );
	defaultMat->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	defaultMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	defaultMat->removeReference();
	SetMaterial( defaultMat );
}

GizmoRenderer::~GizmoRenderer ( void )
{
	delete_safe_array(m_modeldata.vertices);
	delete_safe_array(m_modeldata.triangles);
}

bool GizmoRenderer::PreRender ( void )
{
	const uint kMaxVertices = 4096;
	const uint kMaxTris		= kMaxVertices / 2; // We're doing 100% quads.
	const uint kMaxObjects	= kMaxVertices / 16;

	// Error check now:
	if (EditorObject::Objects().size() > kMaxObjects)
	{	// todo: max this less likely to crash and eat shit
		throw core::OutOfMemoryException();
	}

	// Allocate data for the streamed mesh
	if (m_modeldata.triangles == NULL)
	{
		delete[] m_modeldata.triangles;
		delete[] m_modeldata.vertices;

		m_modeldata.triangles = new arModelTriangle [kMaxTris];
		m_modeldata.vertices = new arModelVertex [kMaxVertices];

		memset(m_modeldata.vertices, 0, sizeof(arModelVertex) * kMaxVertices);
	}
	// Reset count
	m_modeldata.triangleNum = 0;
	m_modeldata.vertexNum = 0;

	// Begin rendering the area rects
	const Color kDefaultColor	(0.6F,0.6F,0.6F,0.4F);
	const Color kGlowColor		(0.9F,0.9F,0.9F,0.4F);
	const Color kSelectColor	(1.0F,1.0F,1.0F,0.8F);
	Color l_typeColor;
	Color l_currentColor;

	// Do a render of all objects
	for ( auto object = EditorObject::Objects().begin(); object != EditorObject::Objects().end(); ++object )
	{
		Rect rect = (*object)->GetSpriteRect();
		rect.pos += (*object)->position;
		Vector3d points [4];
		points[0] = rect.pos + Vector3d( 0,0,transform.world.position.z );
		points[2] = rect.pos + rect.size + Vector3d( 0,0,transform.world.position.z );
		points[1] = Vector3d( points[2].x, points[0].y, points[0].z );
		points[3] = Vector3d( points[0].x, points[2].y, points[0].z );

		l_typeColor = Color( 1.0F,1.0F,1.0F, 1.0F );

		if ( *object == m_target_selection )
			l_currentColor = kSelectColor * l_typeColor;
		else if ( *object == m_target_glow )
			l_currentColor = kGlowColor * l_typeColor;
		else
			continue;

		Vector3d meshpoints [4];

		// Draw the four quads around the edge of the area
		meshpoints[0] = points[0];
		meshpoints[1] = points[1];
		meshpoints[2] = points[1]+Vector2f(0,2);
		meshpoints[3] = points[0]+Vector2f(0,2);
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		meshpoints[0] = points[3]-Vector2f(0,2);
		meshpoints[1] = points[2]-Vector2f(0,2);
		meshpoints[2] = points[2];
		meshpoints[3] = points[3];
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		meshpoints[0] = points[0];
		meshpoints[1] = points[3];
		meshpoints[2] = points[3]+Vector2f(2,0);
		meshpoints[3] = points[0]+Vector2f(2,0);
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		meshpoints[0] = points[1]-Vector2f(2,0);
		meshpoints[1] = points[2]-Vector2f(2,0);
		meshpoints[2] = points[2];
		meshpoints[3] = points[1];
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
	}

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::PreRender();
}