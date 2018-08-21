#ifndef RENDERER_MATERIAL_PASS_H_
#define RENDERER_MATERIAL_PASS_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/material/RrShader.cbuffers.h"
#include "renderer/gpuw/Public/Enums.h"
#include "renderer/material/ShaderSlots.h"

class RrShaderProgram;
class RrTexture;
namespace gpu
{
	class Texture;
}

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
};

enum rrPassConstants : int
{
	kPass_MaxPassCount		= 4,
	kPass_MaxTextureSlots	= 7,
};

enum rrPassOrderConstants : int
{
	kPassOrder_Step					= 10,
	kPassOrder_DepthMaskDisabled	= 1000,
	kPassOrder_PostProcess			= 2000,
};

// 
class RrPass
{
public:
	RENDER_API				RrPass ( void );
	//RENDER_API				RrPass ( const RrPass& other );
	// will release references of owned textures & shaders
	RENDER_API				~RrPass ( void );

	//	setTexture ( slot, texture ) : Sets material texture.
	// Material is given ownership of the texture.
	// Do not delete the texture directly, use RemoveReference.
	RENDER_API void			setTexture ( const rrTextureSlot slot, RrTexture* texture );
	//	setTexture ( slot, texture ) : Sets material texture with raw GPU handles.
	// To be used only in an immediate use-case, ex. post-processing or compositing.
	RENDER_API void			setTexture ( const rrTextureSlot slot, gpu::Texture& n_texture );

	//	setProgram ( program ) : Sets shader program.
	// Pass is given ownership of the program.
	// Do not delete the program directly, use RemoveReference.
	RENDER_API void			setProgram ( RrShaderProgram* program );

	RENDER_API bool			validate ( void );

	//	utilSetupAs2D () : Sets properties of the pass to what all 2D passes typically use.
	// In detail: depth write off, depth test always, no face culling, transparency on, lighting hinted off.
	RENDER_API void			utilSetupAs2D ( void );

public:
	rrPassType			m_type;
	RrShaderProgram*	m_program;		// Shader program. This pass has ownership.
	int					m_orderOffset;	// order offset. see rrPassOrderConstants for util values. lower is earlier

	// General material-like settings:
	renderer::cbuffer::rrPerObjectSurface
						m_surface;
	gpu::CullMode		m_cullMode;

	// Textures:
	RrTexture*			m_textures[kPass_MaxTextureSlots];
	gpu::Texture*		m_texturesRaw[kPass_MaxTextureSlots];

	// Alpha settings:
	renderer::rrAlphaMode
						m_alphaMode;

	// Depth settings:
	bool				m_depthWrite;
	gpu::CompareOp		m_depthTest;
};

#endif//RENDERER_MATERIAL_PASS_H_