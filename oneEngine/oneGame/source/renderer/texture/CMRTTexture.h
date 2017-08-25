#ifndef _C_MULTIPLE_RENDER_TARGET_TEXTURE_H_
#define _C_MULTIPLE_RENDER_TARGET_TEXTURE_H_

// ==Includes==
// CRenderTexture class
#include "CRenderTexture.h"
// Request structure
#include "renderer/types/RrGpuTexture.h"

// ===STRUCTS===
// MRT Info struct
struct tMRTInfo
{
	glHandle		framebuffer;
	glHandle		texture [16];
	glEnum			texture_formats [16];
	bool			texture_owned [16];
	int8_t	attachments;
	int8_t	colorAttachments;
	int8_t	depthAttachment;
	int8_t	stencilAttachment;
};

class CMRTTexture : public CRenderTexture
{
	TextureType( TextureClassRenderTarget_MRT );
public:
	//		Constructor : Create MRT texture with optionally pre-existing textures.
	// requestedWidth, requestedHeight		Size of the desired MRT when creating new textures
	// repeatX, repeatY						Behavior of edges of created textures
	// rgbRequestList						List of textures to attach/generate.
	//											If input handle is zero, a new texture will be generated with the given format.
	// rgbRequestListSize					Number of values in rgbRequestList
	// depthRequest							Pointer to depth texture to attach/generate.
	//											If input handle is zero and depth fetch is enabled, a depth texture will be generated with the given format.
	//											Otherwise, a renderbuffer will be created.
	// depthFetch							When true, uses the input texture. When false, uses a renderbuffer.
	// stencilRequest						Pointer to stencil texture to attach/generate.
	//											If input handle is zero and stencil fetch is enabled, a stencil texture will be generated with the given format.
	//											Otherwise, a renderbuffer will be created.
	// stencilFetch							When true, uses the input texture. When false, uses a renderbuffer.
	explicit CMRTTexture ( 
		unsigned int	requestedWidth,
		unsigned int	requestedHeight,
		eWrappingType	repeatX,
		eWrappingType	repeatY,
		RrGpuTexture*		rgbRequestList,
		const uint		rgbRequestListSize,
		RrGpuTexture*		depthRequest,
		bool			depthFetch,
		RrGpuTexture*		stencilRequest,
		bool			stencilFetch
	);
	~CMRTTexture ( void );

	// Public Binding and Unbinding
	void BindBuffer ( void ) override;
	void UnbindBuffer ( void ) override;

	// Get texture indexer
	unsigned int GetBufferTexture ( int8_t index )
	{
		return mrtInfo.texture[index];
	}


	RENDER_API void SetFilter ( eSamplingFilter filter ) override;

protected:
	// Private routines
	void GenerateFramebuffer ( void ) override;

protected:
	tMRTInfo	mrtInfo;
};

#endif//_C_MULTIPLE_RENDER_TARGET_TEXTURE_H_