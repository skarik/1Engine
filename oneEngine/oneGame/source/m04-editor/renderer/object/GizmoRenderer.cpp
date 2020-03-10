#include "core/math/Rect.h"
#include "core-ext/utils/MeshBuilder.h"

#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"

#include "m04-editor/entities/UIDragHandle.h"
#include "m04-editor/standalone/mapeditor/EditorObject.h"

#include "GizmoRenderer.h"

using namespace M04;

GizmoRenderer::GizmoRenderer ( void )
	: CStreamedRenderable3D()
{
	memset( &m_modeldata, 0, sizeof(arModelData) );

	// Use a default white 2D material
	RrPass spritePass;
	spritePass.utilSetupAsDefault();
	spritePass.m_type = kPassTypeForward;
	spritePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	spritePass.m_cullMode = gpu::kCullModeNone;
	spritePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	spritePass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	spritePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	spritePass.setVertexSpecificationByCommonList(t_vspec, 3);
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	PassInitWithInput(0, &spritePass);
}

GizmoRenderer::~GizmoRenderer ( void )
{
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.indices);
}

bool GizmoRenderer::EndRender ( void )
{
	const uint kMaxVertices = 4096;
	const uint kMaxIndices	= kMaxVertices / 2 * 3; // We're doing 100% quads.
	const uint kMaxObjects	= kMaxVertices / 16;

	// Error check now:
	if (EditorObject::Objects().size() > kMaxObjects)
	{	// todo: max this less likely to crash and eat shit
		throw core::OutOfMemoryException();
	}

	// Allocate data for the streamed mesh
	if (m_modeldata.indices == NULL)
	{
		delete[] m_modeldata.indices;
		delete[] m_modeldata.position;
		delete[] m_modeldata.color;
		delete[] m_modeldata.texcoord0;

		m_modeldata.indices = new uint16_t [kMaxIndices];
		m_modeldata.position = new Vector3f [kMaxVertices];
		m_modeldata.color = new Vector4f [kMaxVertices];
		m_modeldata.texcoord0 = new Vector3f [kMaxVertices];

		memset(m_modeldata.position, 0, sizeof(Vector3f) * kMaxVertices);
	}
	// Reset count
	m_modeldata.indexNum = 0;
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
		Vector3f points [4];
		points[0] = rect.pos + Vector3f( 0,0,transform.world.position.z );
		points[2] = rect.pos + rect.size + Vector3f( 0,0,transform.world.position.z );
		points[1] = Vector3f( points[2].x, points[0].y, points[0].z );
		points[3] = Vector3f( points[0].x, points[2].y, points[0].z );

		l_typeColor = Color( 1.0F,1.0F,1.0F, 1.0F );

		if ( *object == m_target_selection )
			l_currentColor = kSelectColor * l_typeColor;
		else if ( *object == m_target_glow )
			l_currentColor = kGlowColor * l_typeColor;
		else
			continue;

		Vector3f meshpoints [4];

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
	return CStreamedRenderable3D::EndRender();
}