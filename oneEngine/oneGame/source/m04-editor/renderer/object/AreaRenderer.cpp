#include "core-ext/utils/MeshBuilder.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"
#include "engine2d/entities/Area2DBase.h"
#include "AreaRenderer.h"

using namespace M04;

AreaRenderer::AreaRenderer ( void )
	: CStreamedRenderable3D()
{
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
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &spritePass);

	// Start off with empty model data
	memset(&m_modeldata, 0, sizeof(m_modeldata));
}

AreaRenderer::~AreaRenderer ( void )
{
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.indices);
}

bool AreaRenderer::EndRender ( void )
{
	const uint kMaxVertices = 4096;
	const uint kMaxIndices	= kMaxVertices / 6; // We're doing 100% quads.
	const uint kMaxAreas	= kMaxVertices / 32;

	// Error check now:
	if (Engine2D::Area2D::Areas().size() > kMaxAreas)
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

		memset(m_modeldata.position, 0, sizeof(arModelVertex) * kMaxVertices);
	}
	// Reset count
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;

	const Color kDefaultColor	(0.6F,0.6F,0.6F,0.5F);
	const Color kGlowColor		(0.9F,0.9F,0.9F,0.5F);
	const Color kSelectColor	(1.0F,1.0F,1.0F,0.8F);
	Color l_typeColor;
	Color l_currentColor;

	// Do a render of all areas
	for ( auto area = Engine2D::Area2D::Areas().begin(); area != Engine2D::Area2D::Areas().end(); ++area )
	{
		Rect rect = (*area)->m_rect;
		Vector3f points [4];
		points[0] = rect.pos + Vector3f( 0,0,transform.world.position.z );
		points[2] = rect.pos + rect.size + Vector3f( 0,0,transform.world.position.z );
		points[1] = Vector3f( points[2].x, points[0].y, points[0].z );
		points[3] = Vector3f( points[0].x, points[2].y, points[0].z );

		string type = (*area)->GetTypeName();
		if ( type == "Area2DBase" )
			l_typeColor = Color( 0.6F,0.7F,1.0F, 1.0F );
		else if ( type == "AreaTeleport" )
			l_typeColor = Color( 0.9F,1.0F,0.6F, 1.0F );
		else if ( type == "AreaTrigger" )
			l_typeColor = Color( 0.8F,0.4F,0.4F, 1.0F );
		else if ( type == "AreaPlayerSpawn" )
			l_typeColor = Color( 0.5F,1.0F,0.3F, 1.0F );
		else // Error type
			l_typeColor = Color( 1.0F,0.0F,1.0F, 1.0F );

		if ( *area == m_target_selection )
			l_currentColor = kSelectColor * l_typeColor;
		else if ( *area == m_target_glow )
			l_currentColor = kGlowColor * l_typeColor;
		else
			l_currentColor = kDefaultColor * l_typeColor;

		Vector3f meshpoints [4];

		// Draw the four quads around the edge of the area
		meshpoints[0] = points[0];
		meshpoints[1] = points[1];
		meshpoints[2] = points[1]+Vector2f(0,4);
		meshpoints[3] = points[0]+Vector2f(0,4);
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		meshpoints[0] = points[3]-Vector2f(0,4);
		meshpoints[1] = points[2]-Vector2f(0,4);
		meshpoints[2] = points[2];
		meshpoints[3] = points[3];
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		meshpoints[0] = points[0];
		meshpoints[1] = points[3];
		meshpoints[2] = points[3]+Vector2f(4,0);
		meshpoints[3] = points[0]+Vector2f(4,0);
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		meshpoints[0] = points[1]-Vector2f(4,0);
		meshpoints[1] = points[2]-Vector2f(4,0);
		meshpoints[2] = points[2];
		meshpoints[3] = points[1];
		core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());

		// Draw the four corners
		const Vector2f kOffsets [4] = {
			Vector2f(-1,-1), Vector2f(-1,1), Vector2f(1,1), Vector2f(1,-1)
		};
		for ( int i = 0; i < 4; ++i )
		{
			// Build the corner quad
			for ( int n = 0; n < 4; ++n )
			{
				meshpoints[n] = points[i] + kOffsets[n] * 4.0F;
			}
			// If mouse over a corner, highlight the corner
			if ( (*area == m_target_selection || *area == m_target_glow) && i == m_target_corner ) 
				l_currentColor = kSelectColor;
			else 
				l_currentColor = kDefaultColor * l_typeColor;
			// Push the quad
			core::meshbuilder::Quad(&m_modeldata, meshpoints, l_currentColor, Rect());
		}
	}

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::EndRender();
}