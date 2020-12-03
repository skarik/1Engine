#include "renderer/material/RrPass.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"

#include "gpuw/Sampler.h"

#include "core/debug/Console.h"

RrPass::RrPass ( void )
	:
	m_program(NULL), m_textures(), m_texturesRaw(), m_samplers(),
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
	m_blendMode = renderer::kHLBlendModeNone;
	m_depthWrite = true;
	m_depthTest = gpu::kCompareOpLess;
	m_layer = renderer::kRenderLayerWorld;
}

//	utilSetupAs2D () : Sets properties of the pass to what all 2D passes typically use.
// In detail: depth write off, depth test always, no face culling, transparency on, lighting hinted off.
void RrPass::utilSetupAs2D ( void )
{
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

// Copy constructor must increment references of owned textures & shaders
RrPass& RrPass::assignFrom ( const RrPass& other )
{
	*this = other;
	
	for (size_t i = 0; i < kPass_MaxTextureSlots; ++i)
	{
		if (m_textures[i] != NULL)
		{
			m_textures[i]->AddReference();
		}
	}
	if (m_program != NULL)
	{
		m_program->AddReference();
	}

	return *this;
}

//	setTexture ( slot, texture ) : Sets material texture.
// Material is given ownership of the texture.
// Do not delete the texture directly, use RemoveReference.
void RrPass::setTexture ( const rrTextureSlot slot, RrTexture* texture )
{
	ARCORE_ASSERT(slot < kPass_MaxTextureSlots);
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
	ARCORE_ASSERT(slot < kPass_MaxTextureSlots);
	m_texturesRaw[slot] = n_texture;
}

//	setSampler ( slot, samplerInfo ) : Sets and creates sampler object.
void RrPass::setSampler ( const rrTextureSlot slot, gpu::SamplerCreationDescription* scd )
{
	ARCORE_ASSERT(slot < kPass_MaxTextureSlots);
	if (m_samplers[slot] != NULL)
	{
		m_samplers[slot]->destroy(NULL);
		delete(m_samplers[slot]);
	}

	m_samplers[slot] = new gpu::Sampler();
	m_samplers[slot]->create(NULL, scd);
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
		m_vertexSpecification[i].dataOffset = 0;
		m_vertexSpecification[i].binding = renderer::shader::kBinding0 + i;

#	if _ENGINE_DEBUG
		if ((int)m_vertexSpecification[i].location != m_vertexSpecification[i].binding)
		{
			debug::Console->PrintWarning(
				"Mismatch in binding locations with locations (Location %d, binding %d). This is valid, but may be unexpected.\n",
				m_vertexSpecification[i].location,
				m_vertexSpecification[i].binding);
		}
#	endif
		switch (locations[i])
		{
		case renderer::shader::Location::kPosition:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector3f);
			break;
		case renderer::shader::Location::kUV0:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector3f);
			break;
		case renderer::shader::Location::kColor:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32A32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector4f);
			break;
		case renderer::shader::Location::kNormal:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector3f);
			break;
		case renderer::shader::Location::kTangent:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector3f);
			break;
		case renderer::shader::Location::kBinormal:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector3f);
			break;
		case renderer::shader::Location::kUV1:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector3f);
			break;
		case renderer::shader::Location::kBoneWeight:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR32G32B32A32SFloat;
			m_vertexSpecification[i].dataStride = sizeof(Vector4f);
			break;
		case renderer::shader::Location::kBoneIndices:
			m_vertexSpecification[i].dataFormat = gpu::kFormatR16G16B16A16UInteger;
			m_vertexSpecification[i].dataStride = sizeof(uint16_t) * 4;
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