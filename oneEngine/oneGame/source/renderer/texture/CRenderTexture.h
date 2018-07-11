
#ifndef _C_RENDER_TEXTURE_
#define _C_RENDER_TEXTURE_

// ==Includes==
// RrTexture class
#include "RrTexture.h"
// Request structure
#include "renderer/types/RrGpuTexture.h"
// Stack for buffer stack
#include <stack>

// ===STRUCTS===
// RT info struct
struct tRenderTargetInfo
{	
	eDepthFormat	depth;
	eStencilFormat	stencil;

	glHandle		findex;

	bool			fetchcolor;
	bool			fetchdepth;
	bool			fetchstencil;
	glHandle		depthRBO;
	glHandle		depthtex;
	bool			depthowned;
	glHandle		stencilRBO;
	glHandle		stenciltex;
	bool			stencilowned;

	bool			active;
};
// ===CLASS===
// Class definition
class CRenderTexture : public RrTexture
{
	TextureType( core::gfx::tex::kTextureClassRenderTarget );
protected:
	explicit CRenderTexture ( const string& sInFilename )
		: RrTexture( sInFilename )
	{
		;
	}
public:
	/*RENDER_API explicit CRenderTexture ( 
		eColorFormat	format			= RGBA8,
		unsigned int	maxTextureWidth	= 1024,
		unsigned int	maxTextureHeight= 1024,
		eWrappingType	repeatX			= Clamp,
		eWrappingType	repeatY			= Clamp,
		eTextureType	textureType		= Texture2D,
		eDepthFormat	depthType		= DepthNone,
		bool			depthFetch		= false,
		bool			ignoreRGB		= false,
		eStencilFormat	stencilType		= StencilNone,
		bool			stencilFetch	= false
		);*/


	RENDER_API explicit CRenderTexture (
		unsigned int	requestedWidth,
		unsigned int	requestedHeight,
		eWrappingType	repeatX,
		eWrappingType	repeatY,
		eColorFormat	requestedColor,
		eDepthFormat	requestedDepth,
		eStencilFormat	requestedStencil
	);

	//		Constructor : Create render texture with optionally pre-existing textures.
	// requestedWidth, requestedHeight		Size of the desired target when creating new textures
	// repeatX, repeatY						Behavior of edges of created textures
	// requestedColor						Color format for the color that is requested. Set to ColorNone to ignore color rendering.
	// depthRequest							Pointer to depth texture to attach/generate.
	//											If input handle is zero and depth fetch is enabled, a depth texture will be generated with the given format.
	//											Otherwise, a renderbuffer will be created.
	// depthFetch							When true, uses the input texture. When false, uses a renderbuffer.
	// stencilRequest						Pointer to stencil texture to attach/generate.
	//											If input handle is zero and stencil fetch is enabled, a stencil texture will be generated with the given format.
	//											Otherwise, a renderbuffer will be created.
	// stencilFetch							When true, uses the input texture. When false, uses a renderbuffer.
	RENDER_API explicit CRenderTexture (
		unsigned int	requestedWidth,
		unsigned int	requestedHeight,
		eWrappingType	repeatX,
		eWrappingType	repeatY,
		eColorFormat	requestedColor,
		RrGpuTexture		depthRequest,
		bool			depthFetch,
		RrGpuTexture		stencilRequest,
		bool			stencilFetch
	);
	RENDER_API ~CRenderTexture ( void );

	// Public Binding and Unbinding
	virtual void BindBuffer ( void );
	virtual void UnbindBuffer ( void );
	// Unbind All
	static void UnbindBuffer ( char );

	// Public Offtexture binding
	void BindDepth ( void );
	void BindStencil ( void );

	// Public Offtexture grabbing
	glHandle GetDepthSampler ( void ) const {
		return rtInfo.depthtex;
	}
	glHandle GetStencilSampler ( void ) const {
		return rtInfo.stenciltex;
	}

	// Public copy generation
	virtual CRenderTexture* GenerateCopy ( void );

	void Reload ( void ) override {
		;
	}

	// Public property queries
	eColorFormat GetInternalFormat ( void ) const {
		return info.internalFormat;
	};
	// State queue
	const tRenderTargetInfo& GetRTInfo ( void ) const {
		return rtInfo;
	}
	RENDER_API const bool IsValid ( void ) const {
		return rtInfo.active;
	}
protected:
	// Private routines
	virtual void GenerateFramebuffer ( void );

protected:
	tRenderTargetInfo	rtInfo;
	string rtUniqueSId;

protected:
	static std::stack<unsigned int> buffer_stack;
};

#endif