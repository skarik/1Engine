#ifndef RENDERER_RENDER_TEXTURE_H_
#define RENDERER_RENDER_TEXTURE_H_

#include "RrTexture.h"
#include "renderer/types/RrGpuTexture.h"
#include "renderer/gpuw/RenderTargets.h"

//#include <stack>
#include <vector>

// ===STRUCTS===
// RT info struct
/*struct tRenderTargetInfo
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
};*/

//	class RrRenderTexture : Render target class with RrTexture interface for attachment 0.
// Provides a render target with an color output, and utils to add depth and stencil.
// Any attached targets are not owned by the RenderTexture and must be freed by the user.
class RrRenderTexture : public RrTexture
{
	TextureType( core::gfx::tex::kTextureClassRenderTarget );
//protected:
//	explicit RrRenderTexture ( const string& sInFilename )
//		: RrTexture( sInFilename )
//	{
//		;
//	}
//public:
//	/*RENDER_API explicit RrRenderTexture ( 
//		eColorFormat	format			= RGBA8,
//		unsigned int	maxTextureWidth	= 1024,
//		unsigned int	maxTextureHeight= 1024,
//		eWrappingType	repeatX			= Clamp,
//		eWrappingType	repeatY			= Clamp,
//		eTextureType	textureType		= Texture2D,
//		eDepthFormat	depthType		= DepthNone,
//		bool			depthFetch		= false,
//		bool			ignoreRGB		= false,
//		eStencilFormat	stencilType		= StencilNone,
//		bool			stencilFetch	= false
//		);*/
//
//
//	RENDER_API explicit RrRenderTexture (
//		unsigned int	requestedWidth,
//		unsigned int	requestedHeight,
//		eWrappingType	repeatX,
//		eWrappingType	repeatY,
//		eColorFormat	requestedColor,
//		eDepthFormat	requestedDepth,
//		eStencilFormat	requestedStencil
//	);
//
//	//		Constructor : Create render texture with optionally pre-existing textures.
//	// requestedWidth, requestedHeight		Size of the desired target when creating new textures
//	// repeatX, repeatY						Behavior of edges of created textures
//	// requestedColor						Color format for the color that is requested. Set to ColorNone to ignore color rendering.
//	// depthRequest							Pointer to depth texture to attach/generate.
//	//											If input handle is zero and depth fetch is enabled, a depth texture will be generated with the given format.
//	//											Otherwise, a renderbuffer will be created.
//	// depthFetch							When true, uses the input texture. When false, uses a renderbuffer.
//	// stencilRequest						Pointer to stencil texture to attach/generate.
//	//											If input handle is zero and stencil fetch is enabled, a stencil texture will be generated with the given format.
//	//											Otherwise, a renderbuffer will be created.
//	// stencilFetch							When true, uses the input texture. When false, uses a renderbuffer.
//	RENDER_API explicit RrRenderTexture (
//		unsigned int	requestedWidth,
//		unsigned int	requestedHeight,
//		eWrappingType	repeatX,
//		eWrappingType	repeatY,
//		eColorFormat	requestedColor,
//		RrGpuTexture		depthRequest,
//		bool			depthFetch,
//		RrGpuTexture		stencilRequest,
//		bool			stencilFetch
//	);
//	RENDER_API ~RrRenderTexture ( void );

public:
	RENDER_API explicit		RrRenderTexture ( void );
	RENDER_API				~RrRenderTexture ( void );

	RENDER_API bool			Attach ( int slot, RrTexture* texture );
	RENDER_API bool			Attach ( int slot, gpu::Texture* texture );
	RENDER_API bool			Attach ( int slot, gpu::WOFrameAttachment* wof );

	RENDER_API bool			Validate ( void );

	/*
	// Public Binding and Unbinding
	virtual void BindBuffer ( void );
	virtual void UnbindBuffer ( void );
	// Unbind All
	static void UnbindBuffer ( char );

	// Public Offtexture binding
	void BindDepth ( void );
	void BindStencil ( void );
	*/
	/*
	// Public Offtexture grabbing
	glHandle GetDepthSampler ( void ) const {
		return rtInfo.depthtex;
	}
	glHandle GetStencilSampler ( void ) const {
		return rtInfo.stenciltex;
	}
	*/
	// Public copy generation
	//virtual RrRenderTexture* GenerateCopy ( void );

	void					Reload ( void ) override {}

	// Public property queries
	/*eColorFormat GetInternalFormat ( void ) const {
		return info.internalFormat;
	};
	// State queue
	const tRenderTargetInfo& GetRTInfo ( void ) const {
		return rtInfo;
	}
	RENDER_API const bool IsValid ( void ) const {
		return rtInfo.active;
	}*/
protected:
	// Private routines
	//virtual void GenerateFramebuffer ( void );
	std::vector<RrTexture*>
						rrx_attachments;
	std::vector<gpu::Texture>
						tex_attachments;
	std::vector<gpu::WOFrameAttachment>
						wof_attachments;

	enum rrAttachmentType
	{
		kAttachmentType_None,
		kAttachmentType_RrTexture,
		kAttachmentType_GpuTexture,
		kAttachmentType_GpuBuffer,
	};
	struct rrAttachment
	{
		void*				attachment;
		rrAttachmentType	type;
	};
	rrAttachment		m_attachments [18];

	gpu::RenderTarget	m_rt;

protected:
	//tRenderTargetInfo	rtInfo;
	//string rtUniqueSId;

protected:
	//static std::stack<unsigned int> buffer_stack;
};

#endif//RENDERER_RENDER_TEXTURE_H_