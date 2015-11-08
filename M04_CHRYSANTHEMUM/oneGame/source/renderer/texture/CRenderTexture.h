
#ifndef _C_RENDER_TEXTURE_
#define _C_RENDER_TEXTURE_

// ==Includes==
// CTexture class
#include "CTexture.h"
// Stringstream for unique string id generation
#include <sstream>
using std::stringstream;
// Stack for buffer stack
#include <stack>

// ===STRUCTS===
// RT info struct
struct tRenderTargetInfo
{	
	eDepthFormat	depth;
	eStencilFormat	stencil;

	glHandle		findex;
	glHandle		depthRBO;
	glHandle		stencilRBO;

	bool			fetchcolor;
	bool			fetchdepth;
	bool			fetchstencil;
	glHandle		depthtex;
	glHandle		stenciltex;

	bool			active;
};
// ===CLASS===
// Class definition
class CRenderTexture : public CTexture
{
	TextureType( TextureClassRenderTarget );
protected:
	explicit CRenderTexture ( const string& sInFilename )
		: CTexture( sInFilename )
	{
		;
	}
public:
	RENDER_API explicit CRenderTexture ( 
		eInternalFormat	format			= RGBA8,
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
	eInternalFormat GetInternalFormat ( void ) const {
		return info.internalFormat;
	};
	// State queue
	const tRenderTargetInfo& GetRTInfo ( void ) const {
		return rtInfo;
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