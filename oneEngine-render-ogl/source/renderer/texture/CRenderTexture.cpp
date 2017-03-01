
// == Includes ==
#include "CRenderTexture.h"
#include "core/debug/CDebugConsole.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/exceptions.h"
#include "renderer/gpuw/Textures.h"

// Stringstream for unique string id generation
#include <sstream>

using std::stringstream;
using std::cout;
using std::endl;

// Definition of buffer stack
std::stack<unsigned int> CRenderTexture::buffer_stack;

// == Constructor + Destructor ==
// Explicit Constructor


CRenderTexture::CRenderTexture (
	unsigned int	requestedWidth,
	unsigned int	requestedHeight,
	eWrappingType	repeatX,
	eWrappingType	repeatY,
	eColorFormat	requestedColor,
	eDepthFormat	requestedDepth,
	eStencilFormat	requestedStencil
)
	: CRenderTexture(
		requestedWidth, requestedHeight,
		repeatX, repeatY,
		requestedColor,
		glTexture(0, requestedDepth), requestedDepth != DepthNone,
		glTexture(0, requestedStencil), requestedStencil != StencilNone )
{
	// Delegated constructor
}


CRenderTexture::CRenderTexture (
	unsigned int	requestedWidth,
	unsigned int	requestedHeight,
	eWrappingType	repeatX,
	eWrappingType	repeatY,
	eColorFormat	requestedColor,
	glTexture		depthRequest,
	bool			depthFetch,
	glTexture		stencilRequest,
	bool			stencilFetch )
	: CTexture ( "_hx_SYSTEM_RENDERTEXTURE" )
{
	GL_ACCESS; // Using the glMainSystem accessor

	// Check for NPOT texture support, and modify texture size accordingly.
	if ( !GL.NPOTsAvailable )
	{
		unsigned int nWidth		= 2;
		unsigned int nHeight	= 2;

		while ( nWidth < requestedWidth )
		{
			nWidth *= 2;
		}
		while ( nHeight < requestedHeight )
		{
			nHeight *= 2;
		}

		requestedWidth	= nWidth;
		requestedHeight	= nHeight;
	}

	// Set the information structure to prepare for reading in
	info.type			= Texture2D;
	info.internalFormat	= ColorNone;
	info.width			= std::min<uint>( requestedWidth, (unsigned)GL.MaxTextureSize );
	info.height			= std::min<uint>( requestedHeight, (unsigned)GL.MaxTextureSize );
	info.index			= 0;
	info.repeatX		= repeatX;
	info.repeatY		= repeatY;

	rtInfo.fetchcolor	= (requestedColor != ColorNone);
	rtInfo.depth		= DepthNone;
	rtInfo.fetchdepth	= false;
	rtInfo.stencil		= StencilNone;
	rtInfo.fetchstencil	= false;

	// Initialze data
	rtInfo.findex		= 0;
	rtInfo.depthRBO		= 0;
	rtInfo.stencilRBO	= 0;
	rtInfo.active		= false;

	// Generate an id
	stringstream tempstream;
	tempstream << "__hx_rt_" << (void*)this << "_" << info.width << "_" << info.height; 
	rtUniqueSId = string( tempstream.str() );
	// Thing is, this ID isn't really needed, as the way render textures work is much different.

	// Print out infos
	cout << "New render texture: " << rtUniqueSId << endl;

	// Generate the data
	if ( rtInfo.fetchcolor )
	{
		cout << " + Fetch color enabled." << endl;

		// Create a new color texture and set it up
		info.index = GPU::TextureAllocate( Texture2D, requestedColor, info.width, info.height );
		GPU::TextureSampleSettings( Texture2D, info.index, info.repeatX, info.repeatY, 0, SamplingLinear, SamplingLinear );
		GL.CheckError();

		// Update color format
		info.internalFormat = requestedColor;
	}
	else
	{
		info.index = 0;
	}

	// Do the same if there's a renderbuffer
	if ( depthFetch )
	{
		cout << " + Fetch depth enabled." << endl;

		if ( depthRequest.texture == 0 )
		{
			// Create a new depth texture and set it up
			rtInfo.depthtex = GPU::TextureAllocate( Texture2D, depthRequest.format, info.width, info.height );
			GPU::TextureSampleSettings( Texture2D, rtInfo.depthtex, info.repeatX, info.repeatY, 0, SamplingLinear, SamplingLinear );
			GL.CheckError();

			// Update color format
			rtInfo.depth = (eDepthFormat)depthRequest.format;
			rtInfo.depthowned = true;
		}
		else
		{
			rtInfo.depthtex = depthRequest.texture;
			rtInfo.depth = (eDepthFormat)depthRequest.format;
			rtInfo.depthowned = false;
		}
	}
	else
	{
		rtInfo.depthtex = 0;
		if ( depthRequest.texture == 0 )
		{
			rtInfo.depth = (eDepthFormat)depthRequest.format;
			if ( rtInfo.depth != DepthNone )
				rtInfo.depthRBO = GPU::TextureBufferAllocate(Texture2D, rtInfo.depth, info.width, info.height);
			rtInfo.depthowned = true;
		}
		else
		{
			rtInfo.depth = (eDepthFormat)depthRequest.format;
			rtInfo.depthRBO = depthRequest.texture;
			rtInfo.depthowned = false;
		}
	}

	// Do the same if there's a stencilbuffer
	if ( stencilFetch )
	{
		cout << " + Fetch stencil enabled." << endl;

		if ( stencilRequest.texture == 0 )
		{
			// Create a new stencil texture and set it up
			rtInfo.stenciltex = GPU::TextureAllocate( Texture2D, stencilRequest.format, info.width, info.height );
			GPU::TextureSampleSettings( Texture2D, rtInfo.stenciltex, info.repeatX, info.repeatY, 0, SamplingLinear, SamplingLinear );
			GL.CheckError();

			// Update color format
			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
			rtInfo.stencilowned = true;
		}
		else
		{
			rtInfo.stenciltex = stencilRequest.texture;
			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
			rtInfo.stencilowned = false;
		}
	}
	else
	{
		rtInfo.stenciltex = 0;
		if ( stencilRequest.texture == 0 )
		{
			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
			if ( rtInfo.stencil != StencilNone )
				rtInfo.stencilRBO = GPU::TextureBufferAllocate(Texture2D, rtInfo.stencil, info.width, info.height);
			rtInfo.stencilowned = true;
		}
		else
		{
			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
			rtInfo.stencilRBO = stencilRequest.texture;
			rtInfo.stencilowned = false;
		}
	}

	// And now create the framebuffer
	GenerateFramebuffer();

	// Make sure the buffer stack is not empty
	if ( buffer_stack.empty() )
		buffer_stack.push(0);
}

// Destructor
CRenderTexture::~CRenderTexture ( void )
{
	// Free framebuffer
	if ( rtInfo.findex )
		glDeleteFramebuffers( 1, &rtInfo.findex );

	// Free renderbuffers
	if ( rtInfo.depthRBO && rtInfo.depthowned )
		glDeleteRenderbuffers( 1, &rtInfo.depthRBO );
	if ( rtInfo.stencilRBO && rtInfo.stencilowned )
		glDeleteRenderbuffers( 1, &rtInfo.stencilRBO );

	// Free textures
	if ( info.index )
		glDeleteTextures( 1, &info.index );
	if ( rtInfo.depthtex && rtInfo.depthowned )
		glDeleteTextures( 1, &rtInfo.depthtex );
	if ( rtInfo.stenciltex && rtInfo.stencilowned )
		glDeleteTextures( 1, &rtInfo.stenciltex );
}

// == Generating Identical-sized Buffers ==
// Public copy generation
CRenderTexture* CRenderTexture::GenerateCopy ( void )
{
	/*return new CRenderTexture(
		info.internalFormat, info.width, info.height,
		info.repeatX, info.repeatY, info.type,
		rtInfo.depth, rtInfo.fetchdepth, !rtInfo.fetchcolor, rtInfo.stencil, rtInfo.fetchstencil );*/
	return new CRenderTexture(
		info.width, info.height,
		info.repeatX, info.repeatY,
		info.internalFormat,
		glTexture(0, rtInfo.depth), rtInfo.depthtex != 0,
		glTexture(0, rtInfo.stencil), rtInfo.stenciltex != 0 );
}

// == Texture binding ==
void CRenderTexture::BindDepth ( void )
{
#ifdef _ENGINE_DEBUG
	if ( rtInfo.depthtex == 0 )
		throw Renderer::InvalidOperationException();
#endif
	glBindTexture( GL_TEXTURE_2D, rtInfo.depthtex );
}
void CRenderTexture::BindStencil ( void )
{
#ifdef _ENGINE_DEBUG
	if ( rtInfo.stenciltex == 0 )
		throw Renderer::InvalidOperationException();
#endif
	glBindTexture( GL_TEXTURE_2D, rtInfo.stenciltex );
}

// == Public Bind and Unbind ==
// Bind buffer to this guy's FBO
void CRenderTexture::BindBuffer ( void )
{
	if ( rtInfo.active )
	{
		// set rendering destination to FBO
		glBindFramebuffer( GL_FRAMEBUFFER, rtInfo.findex );
		if ( rtInfo.fetchcolor ) {
			GLuint bufferCount [] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers( 1, bufferCount );
		}
		buffer_stack.push( rtInfo.findex );
	}
	else
	{
		cout << "Warning: Attempted to bind inactive buffer " << rtUniqueSId << endl;
		cout << "   Make sure FBO's are supported and your latest OpenGL drivers are installed." << endl;
	}
}

// Unbind, meaning bind buffer back to default
void CRenderTexture::UnbindBuffer ( void )
{
	// Bind buffer back to default
	if ( !buffer_stack.empty() )
	{
		if ( buffer_stack.top() != rtInfo.findex )
			cout << "ERROR! MISMATCHED BUFFER STACK!" << endl;
		buffer_stack.pop();
	}
	if ( buffer_stack.empty() ) {
		Debug::Console->PrintError( "ERROR! IMBALANCED BUFFER STACK!\n" );
	}
	else {
		glBindFramebuffer( GL_FRAMEBUFFER, buffer_stack.top() ); // TODO: MOVE TO FRAMEBUFFER STACK
	}
	//cout << buffer_stack.size() << endl;
}
// Global Unbind
void CRenderTexture::UnbindBuffer ( char )
{
	// Bind buffer back to default
	if ( !buffer_stack.empty() ) {
		buffer_stack.pop();
	}
	if ( buffer_stack.empty() ) {
		Debug::Console->PrintError( "ERROR! IMBALANCED BUFFER STACK!\n" );
	}
	else {
		glBindFramebuffer( GL_FRAMEBUFFER, buffer_stack.top() );
	}
}

// == Private Routines ==
// Generate framebuffer. This checks if the current framebuffer is valid. If not, it'll create a new framebuffer.
void CRenderTexture::GenerateFramebuffer ( void )
{
	GL_ACCESS; // Using the glMainSystem accessor

	// Ignore this if there's no FBO support
	if ( !GL.FBOsAvailable )
	{
		rtInfo.active = false;
		return;
	}
	if ( !rtInfo.active )
	{
		// create a framebuffer object
		glGenFramebuffers( 1, &rtInfo.findex );
		glBindFramebuffer( GL_FRAMEBUFFER, rtInfo.findex );

		if ( rtInfo.fetchcolor )
		{
			cout << " + Attaching color texture." << endl;
			// attach the texture to FBO color attachment point
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, info.index, 0 );
		}
		else
		{
			// Need to turn color off
			// NVidia
			{
				glDrawBuffer( GL_NONE );
				glReadBuffer( GL_NONE );
			}
			// ATI ?
			{
				//glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
				//glFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, info.index, 0 );
			}
		}

		// Bind the render buffer
		if ( rtInfo.depthRBO != 0 )
		{
			cout << " + Attaching depth RBO." << endl;
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtInfo.depthRBO );
		}
		else if ( rtInfo.depthtex != 0 )
		{
			cout << " + Attaching depth texture." << endl;
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rtInfo.depthtex, 0 );
		}

		// Bind the stencil buffer
		if ( rtInfo.stencilRBO != 0 )
		{
			cout << " + Attaching stencil RBO." << endl;
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rtInfo.stencilRBO );
		}
		else if ( rtInfo.stenciltex != 0 )
		{
			cout << " + Attaching stencil texture." << endl;
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rtInfo.stenciltex, 0 );
		}

		// check FBO status
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if ( status != GL_FRAMEBUFFER_COMPLETE )
		{
			rtInfo.active = false;

			switch ( status )
			{
			case GL_FRAMEBUFFER_UNDEFINED:
				cout << "   bufferStatus: GL_FRAMEBUFFER_UNDEFINED" << endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << endl; break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				cout << "   bufferStatus: GL_FRAMEBUFFER_UNSUPPORTED" << endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << endl; break;
			}
		}
		else {
			rtInfo.active = true;
		}

		// switch back to window-system-provided framebuffer
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		// Print out success
		if ( rtInfo.active )
			cout << " + Render texture " << rtUniqueSId << " is now active." << endl;
		else
			cout << " - FAILURE: Render texture " << rtUniqueSId << " failed to activate." << endl;
	}
}

/*
bool checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "Framebuffer complete." << std::endl;
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
        return false;

    default:
        std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
        return false;
    }
}*/