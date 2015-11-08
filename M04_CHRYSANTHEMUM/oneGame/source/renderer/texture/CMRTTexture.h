
#ifndef _C_MULTIPLE_RENDER_TARGET_TEXTURE_H_
#define _C_MULTIPLE_RENDER_TARGET_TEXTURE_H_

// ==Includes==
// CRenderTexture class
#include "CRenderTexture.h"

// ===STRUCTS===
// MRT Info struct
struct tMRTInfo
{
	unsigned int	fb_indices [16];
	unsigned int	tx_indices [16];
	int8_t	colorAttachments;
	int8_t	depthAttachment;
	int8_t	stencilAttachment;
};

class CMRTTexture : public CRenderTexture
{
	TextureType( TextureClassRenderTarget_MRT );
public:
	explicit CMRTTexture ( 
		eInternalFormat	format			= RGBA8,
		unsigned int	maxTextureWidth	= 1024,
		unsigned int	maxTextureHeight= 1024,
		eWrappingType	repeatX			= Clamp,
		eWrappingType	repeatY			= Clamp,
		eDepthFormat	depthType		= DepthNone,
		bool			depthFetch		= true,
		eStencilFormat	stencilType		= StencilNone,
		bool			stencilFetch	= true,
		uint			colorAttachments= 1
		);
	~CMRTTexture ( void );

	// Public Binding and Unbinding
	void BindBuffer ( void ) override;
	void UnbindBuffer ( void ) override;

	// Get texture indexer
	unsigned int GetBufferTexture ( int8_t index ) {
		return mrtInfo.tx_indices[index];
	}

protected:
	// Private routines
	void GenerateFramebuffer ( void ) override;

protected:
	tMRTInfo	mrtInfo;
};

#endif//_C_MULTIPLE_RENDER_TARGET_TEXTURE_H_