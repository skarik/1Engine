#include "renderer/material/RrPass.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"

RrPass::RrPass ( void )
	:
	m_program(NULL), m_textures(), m_texturesRaw(),
	m_vertexSpecification(NULL), m_vertexSpecificationCount(0)
{
	utilSetupAsDefault();
}

//	utilSetupAsDefault () : Sets proprties of the pass to what the default 3D pass usually is.
// In detail: depth write on, depth test, face culling, transparency off, world layer
void RrPass::utilSetupAsDefault ( void )
{
	m_type = kPassTypeDeferred;
	m_orderOffset = 0;
	m_surface = renderer::cbuffer::rrPerObjectSurface();
	m_cullMode = gpu::kCullModeBack;
	m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	m_alphaMode = renderer::kAlphaModeNone;
	m_depthWrite = true;
	m_depthTest = gpu::kCompareOpLess;
	m_layer = renderer::kRenderLayerWorld;
}

//	utilSetupAs2D () : Sets properties of the pass to what all 2D passes typically use.
// In detail: depth write off, depth test always, no face culling, transparency on, lighting hinted off.
void RrPass::utilSetupAs2D ( void )
{
	//m_lighting_mode = renderer::LI_NONE;
	m_alphaMode = renderer::kAlphaModeTranslucent;
	m_cullMode = gpu::kCullModeNone;
	m_depthWrite = false;
	m_depthTest = gpu::kCompareOpAlways;
}

// Destructor for the pass attempts to release references to all objects in use.
// This should only include Rr Textures & Shader programs.
RrPass::~RrPass ( void )
{
	for (size_t i = 0; i < kPass_MaxTextureSlots; ++i)
	{
		if (m_textures[i] != NULL)
		{
			m_textures[i]->RemoveReference();
		}
	}
	if (m_program != NULL)
	{
		m_program->RemoveReference();
	}
}

//	setTexture ( slot, texture ) : Sets material texture.
// Material is given ownership of the texture.
// Do not delete the texture directly, use RemoveReference.
void RrPass::setTexture ( const rrTextureSlot slot, RrTexture* texture )
{
	if (m_textures[slot] != texture)
	{
		// Free the slot.
		if (m_textures[slot] != NULL)
			m_textures[slot]->RemoveReference();

		// Assign the slot.
		m_textures[slot] = texture;
		if (m_textures[slot] != NULL)
			m_textures[slot]->AddReference();

		// Null out the direct mode slot
		m_texturesRaw[slot] = NULL;
	}
}

//	setTexture ( slot, texture ) : Sets material texture with raw GPU handles.
// To be used only in an immediate use-case, ex. post-processing or compositing.
// Do not ever use for normal objects due to memory considerations.
void RrPass::setTexture ( const rrTextureSlot slot, gpu::Texture* n_texture )
{
	m_texturesRaw[slot] = n_texture;
}

//	setProgram ( program ) : Sets shader program.
// Pass is given ownership of the program.
// Do not delete the program directly, use RemoveReference.
void RrPass::setProgram ( RrShaderProgram* program )
{
	if (m_program != program)
	{
		// Free the program
		if (m_program != NULL)
			m_program->RemoveReference();

		// Assign the slot.
		m_program = program;
		if (m_program != NULL)
			m_program->AddReference();
	}
}

//	setVertexSpecification ( ... )
void RrPass::setVertexSpecification ( renderer::shader::VertexAttribute* attributes, const size_t attribute_count )
{
	m_vertexSpecification = new renderer::shader::VertexAttribute[attribute_count];
	memcpy(m_vertexSpecification, attributes, sizeof(renderer::shader::VertexAttribute) * attribute_count);
	m_vertexSpecificationCount = (int)attribute_count;
}

//	setVertexSpecificationByCommonList ( ... )
void RrPass::setVertexSpecificationByCommonList ( renderer::shader::Location* locations, const size_t attribute_count )
{
	m_vertexSpecification = new renderer::shader::VertexAttribute[attribute_count];
	for (size_t i = 0; i < attribute_count; ++i)
	{
		m_vertexSpecification[i] = renderer::shader::VertexAttribute();
		m_vertexSpecification[i].location = locations[i];
		switch (locations[i])
		{
		case renderer::shader::Location::kPosition:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding0;
			break;
		case renderer::shader::Location::kUV0:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding1;
			break;
		case renderer::shader::Location::kColor:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32A32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding2;
			break;
		case renderer::shader::Location::kNormal:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding3;
			break;
		case renderer::shader::Location::kTangent:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding4;
			break;
		case renderer::shader::Location::kBinormal:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding5;
			break;
		case renderer::shader::Location::kUV1:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding6;
			break;
		case renderer::shader::Location::kBoneWeight:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32A32SFloat;
			m_vertexSpecification[i].binding = renderer::shader::kBinding7;
			break;
		case renderer::shader::Location::kBoneIndices:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR16G16B16A16UInteger;
			m_vertexSpecification[i].binding = renderer::shader::kBinding8;
			break;
		case renderer::shader::Location::kIgnore:
		case renderer::shader::Location::kInvalid:
			ARCORE_ERROR("Invalid vertex attribute location specified!");
			break;
		default:
			ARCORE_ERROR("Unsupported vertex attribute location specified!");
			break;
		}
	}
	m_vertexSpecificationCount = (int)attribute_count;
}

//	validate ( ... ) 
bool RrPass::validate ( void )
{
	return m_vertexSpecification != NULL && m_program != NULL;
}