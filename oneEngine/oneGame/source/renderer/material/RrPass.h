#ifndef RENDERER_MATERIAL_PASS_H_
#define RENDERER_MATERIAL_PASS_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/types/shaders/cbuffers.h"
#include "renderer/types/shaders/sbuffers.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Sampler.h"
#include "renderer/material/ShaderSlots.h"
#include "renderer/material/VertexAttribute.h"
#include "renderer/types/Shading.h"

class RrShaderProgram;
class RrTexture;
namespace gpu
{
	class Texture;
	class Sampler;
}

typedef void (*rrPassRenderFunction)(gpu::GraphicsContext*);

enum rrPassType
{
	// World pass, forward rendered.
	kPassTypeForward,
	// World pass, deferred rendered.
	kPassTypeDeferred,
	// World pass, volume fog. 
	// Uses the depth prepass to set up.
	kPassTypeVolumeFog,
	// World pass, warp.
	// Uses the depth prepass to set up.
	kPassTypeWarp,
	// System pass, lighting shadows, forward rendered.
	kPassTypeShadowColorOverride,

	// General purpose job pass. Renders outside of viewport sets, so safe render-target work can happen.
	// Use for items where the sorting doesn't quite matter.
	kPassTypeJob,

	// System pass used for rendering depth.
	kPassTypeSystemDepth,
};

enum rrPassConstants : int
{
	kPass_MaxPassCount		= 4,
	kPass_MaxTextureSlots	= 16,
};

enum rrPassOrderConstants : int
{
	kPassOrder_Step					= 10,
	kPassOrder_DepthMaskDisabled	= 1000,
	kPassOrder_PostProcess			= 2000,
	kPassOrder_DebugTools			= 9000,
};

enum rrPassProgramFor : int
{
	kPassProgramForEverything	= 0,
	kPassProgramForDepth		= 1,
};

// RrPass, defines a single pass for any object.
// An RrPass stores information used to generate a gpu::Pipeline by the renderer.
class RrPass
{
protected:
	// will not increment references of owned textures & shaders
	RENDER_API RrPass&		operator= ( const RrPass& other ) = default;
public:
	RENDER_API explicit		RrPass ( void );
	// will release references of owned textures & shaders
	RENDER_API				~RrPass ( void );
	// will increment references of owned textures & shaders
	RENDER_API RrPass&		assignFrom ( const RrPass& other );

	//	setTexture ( slot, texture ) : Sets material texture.
	// Material is given ownership of the texture.
	// Do not delete the texture directly, use RemoveReference.
	RENDER_API void			setTexture ( const rrTextureSlot slot, RrTexture* texture );
	//	setTexture ( slot, texture ) : Sets material texture with raw GPU handles.
	// To be used only in an immediate use-case, ex. post-processing or compositing.
	// Do not ever use for normal objects due to memory considerations.
	RENDER_API void			setTexture ( const rrTextureSlot slot, gpu::Texture* n_texture );

	//	setSampler ( slot, samplerInfo ) : Sets and creates sampler object.
	RENDER_API void			setSampler ( const rrTextureSlot slot, gpu::SamplerCreationDescription* scd );

	//	setProgram ( program ) : Sets shader program.
	// Pass is given ownership of the program.
	// Do not delete the program directly, use RemoveReference.
	RENDER_API void			setProgram ( RrShaderProgram* program );
	//	setProgram ( program, render_type ) : Sets shader program for the given rendering type.
	// Pass is given ownership of the program.
	// Do not delete the program directly, use RemoveReference.
	RENDER_API void			setProgram ( RrShaderProgram* program, rrPassProgramFor render_type );

	//	setVertexSpecification ( ... )
	RENDER_API void			setVertexSpecification ( renderer::shader::VertexAttribute* attributes, const size_t attribute_count );
	//	setVertexSpecificationByCommonList ( ... )
	RENDER_API void			setVertexSpecificationByCommonList ( renderer::shader::Location* locations, const size_t attribute_count );

	//	validate ( ... ) 
	RENDER_API bool			validate ( void );

	//	utilSetupAsDefault () : Sets proprties of the pass to what the default 3D pass usually is.
	// In detail: depth write on, depth test, face culling, transparency off, world layer
	RENDER_API void			utilSetupAsDefault ( void );
	//	utilSetupAs2D () : Sets properties of the pass to what all 2D passes typically use.
	// In detail: depth write off, depth test always, no face culling, transparency on, lighting hinted off.
	RENDER_API void			utilSetupAs2D ( void );

public:
	//	isTranslucent () : Is this pass to be considered as part of the translucent passgroup?
	RENDER_API bool			isTranslucent ( void ) const
	{
		return m_alphaMode == renderer::kAlphaModeTranslucent
			|| m_blendMode == renderer::kHLBlendModeAdd
			|| m_blendMode == renderer::kHLBlendModeInvMultiply
			|| m_blendMode == renderer::kHLBlendModeMultiply
			|| m_blendMode == renderer::kHLBlendModeMultiplyX2
			|| m_blendMode == renderer::kHLBlendModeSoftAdd;
	}

public:
	rrPassType			m_type;
	RrShaderProgram*	m_program;		// Shader program. This pass has ownership.
	int					m_orderOffset;	// order offset. see rrPassOrderConstants for util values. lower is earlier

	// General material-like settings:
	renderer::cbuffer::rrPerObjectSurface
						m_surface;
	gpu::CullMode		m_cullMode;

	// Vertex shader settings:
	gpu::PrimitiveTopology
						m_primitiveType;
	renderer::shader::VertexAttribute*
						m_vertexSpecification;
	int					m_vertexSpecificationCount;

	// Textures:
	RrTexture*			m_textures[kPass_MaxTextureSlots];
	gpu::Texture*		m_texturesRaw[kPass_MaxTextureSlots];
	gpu::Sampler*		m_samplers[kPass_MaxTextureSlots];

	// Alpha settings:
	renderer::rrAlphaMode
						m_alphaMode = renderer::kAlphaModeNone;

	// Blend settings:
	renderer::rrHLBlendMode
						m_blendMode = renderer::kHLBlendModeNone;	// Defaults to kHLBlendModeNone. If set otherwise, will override m_alphaMode.

	// Depth settings:
	bool				m_overrideDepth = false;
	bool				m_overrideDepthWrite = true;
	gpu::CompareOp		m_overrideDepthTest = gpu::kCompareOpLess;

	// World settings:
	renderer::rrRenderLayer
						m_layer = renderer::kRenderLayerWorld;	// Render layer

	// Special render callbacks:
	rrPassRenderFunction
						m_renderCallback = nullptr;

public:
	// External accessor used to go through data that can be safely modified after Pass is set in an object.
	class SafeAccessor
	{
	public:
		RENDER_API				SafeAccessor ( RrPass* pass )
			: m_pass(pass)
			{}

		//	setTexture ( slot, texture ) : Sets material texture.
		// Material is given ownership of the texture.
		// Do not delete the texture directly, use RemoveReference.
		RENDER_API void			setTexture ( const rrTextureSlot slot, RrTexture* texture )
			{ m_pass->setTexture(slot, texture); }
		//	setTexture ( slot, texture ) : Sets material texture with raw GPU handles.
		// To be used only in an immediate use-case, ex. post-processing or compositing.
		// Do not ever use for normal objects due to memory considerations.
		RENDER_API void			setTexture ( const rrTextureSlot slot, gpu::Texture* n_texture )
			{ m_pass->setTexture(slot, n_texture); }

		//	setSampler ( slot, samplerInfo ) : Sets and creates sampler object.
		RENDER_API void			setSampler ( const rrTextureSlot slot, gpu::SamplerCreationDescription* scd )
			{ m_pass->setSampler(slot, scd); }

		//	setHLBlendMode ( blendMode ) : Sets material blend mode.
		// If not set to kHLBlendModeNone, will override any alpha mode.
		RENDER_API void			setHLBlendMode ( const renderer::rrHLBlendMode blendMode )
			{ m_pass->m_blendMode = blendMode; }

		//	getVertexSpecificationCount () : Get how many vertex spec count
		RENDER_API int			getVertexSpecificationCount ( void ) const
			{ return m_pass->m_vertexSpecificationCount; }

		//	getVertexSpecification () : Get the vertex specification info, for binding vbuffers
		RENDER_API const renderer::shader::VertexAttribute*
								getVertexSpecification ( void ) const
			{ return m_pass->m_vertexSpecification; }

	private:
		RrPass*				m_pass;
	};
};

#endif//RENDERER_MATERIAL_PASS_H_