
// == Includes ==
#include "CRenderTexture.h"
#include "core/debug/CDebugConsole.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/exceptions.h"

using std::cout;
using std::endl;

// Definition of buffer stack
std::stack<unsigned int> CRenderTexture::buffer_stack;

// == Constructor + Destructor ==
// Explicit Constructor
CRenderTexture::CRenderTexture ( 
		eInternalFormat	format,
		unsigned int	maxTextureWidth,
		unsigned int	maxTextureHeight,
		eWrappingType	repeatX,
		eWrappingType	repeatY,
		eTextureType	textureType,
		eDepthFormat	depthType,
		bool			depthFetch,
		bool			ignoreRGB,
		eStencilFormat	stencilType,
		bool			stencilFetch )
	: CTexture ( "_hx_SYSTEM_RENDERTEXTURE" )
{
	GL_ACCESS; // Using the glMainSystem accessor

	// Check for NPOT texture support, and modify texture size accordingly.
	if ( !GL.NPOTsAvailable )
	{
		unsigned int nWidth		= 2;
		unsigned int nHeight	= 2;

		while ( nWidth < maxTextureWidth )
		{
			nWidth *= 2;
		}
		while ( nHeight < maxTextureHeight )
		{
			nHeight *= 2;
		}
	
		maxTextureWidth		= nWidth;
		maxTextureHeight	= nHeight;
	}

	// Set the information structure to prepare for reading in
	info.type			= textureType;
	info.internalFormat	= format;
	info.width			= std::min<uint>( maxTextureWidth, (unsigned)GL.MaxTextureSize );
	info.height			= std::min<uint>( maxTextureHeight, (unsigned)GL.MaxTextureSize );
	info.index			= 0;
	info.repeatX		= repeatX;
	info.repeatY		= repeatY;
	
	rtInfo.fetchcolor	= !ignoreRGB;
	rtInfo.depth		= depthType;
	rtInfo.fetchdepth	= depthFetch;
	rtInfo.stencil		= stencilType;
	rtInfo.fetchstencil	= stencilFetch;

	// Initialze data
	rtInfo.findex		= 0;
	rtInfo.depthRBO		= 0;
	rtInfo.stencilRBO	= 0;
	rtInfo.active		= false;

	// Generate an id
	stringstream tempstream;
	tempstream << "__hx_rt_" << (void*)this << "_" << info.width << "_" << info.height; 
	rtUniqueSId = string( tempstream.str() );
	//sFilename = rtUniqueSId;
	// Thing is, this ID isn't really needed, as the way render textures work is much different.

	// Print out infos
	cout << "New render texture: " << rtUniqueSId << endl;

	// Generate the data
	if ( rtInfo.fetchcolor )
	{
		cout << " + Fetch color enabled." << endl;
		// Send the data to OpenGL
		//info.index = GL.GetNewTexture();
		glGenTextures( 1, &info.index );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, info.index );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// Turn on automatic mipmap generation
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		// Copy the data to the texture object
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		// Unbind the data
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		info.index = 0;
		rtInfo.findex = 0;
	}

	// Do the same if there's a renderbuffer
	if (( rtInfo.depth != DepthNone )&&( rtInfo.fetchdepth ))
	{
		cout << " + Fetch depth enabled." << endl;
		// Send the data to OpenGL
		//rtInfo.depthtex = GL.GetNewTexture();
		glGenTextures( 1, &rtInfo.depthtex );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, rtInfo.depthtex );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// Turn on automatic mipmap generation
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		// Copy the data to the texture object
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(rtInfo.depth), info.width, info.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
		// Unbind the data
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		rtInfo.depthtex = 0;
		rtInfo.depthRBO = 0;
	}

	// Do the same if there's a stencilbuffer
	if (( rtInfo.stencil != StencilNone )&&( rtInfo.fetchstencil ))
	{
		cout << " + Fetch stencil enabled." << endl;
		// Send the data to OpenGL
		//rtInfo.stenciltex = GL.GetNewTexture();
		glGenTextures( 1, &rtInfo.stenciltex );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, rtInfo.stenciltex );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// Turn on automatic mipmap generation
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		// Copy the data to the texture object
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(rtInfo.stencil), info.width, info.height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0 );
		// Unbind the data
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		rtInfo.stenciltex = 0;
		rtInfo.stencilRBO = 0;
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
	if ( rtInfo.depthRBO ) {
		/*if ( rtInfo.fetchdepth )
			glDeleteFramebuffers( 1, &rtInfo.depthRBO );
		else*/
		if ( !rtInfo.fetchdepth )
			glDeleteRenderbuffers( 1, &rtInfo.depthRBO );
	}
	if ( rtInfo.stencilRBO ) {
		/*if ( rtInfo.fetchstencil )
			glDeleteFramebuffers( 1, &rtInfo.stencilRBO );
		else*/
		if ( !rtInfo.fetchstencil )
			glDeleteRenderbuffers( 1, &rtInfo.stencilRBO );
	}
	// Free texture
	if ( info.index )
		glDeleteTextures( 1, &info.index );
		//GL.FreeTexture( info.index );
	if ( rtInfo.depthtex )
		glDeleteTextures( 1, &rtInfo.depthtex );
		//GL.FreeTexture( rtInfo.depthtex );
	if ( rtInfo.stenciltex )
		glDeleteTextures( 1, &rtInfo.stenciltex );
		//GL.FreeTexture( rtInfo.stenciltex );
}

// == Generating Identical-sized Buffers ==
// Public copy generation
CRenderTexture* CRenderTexture::GenerateCopy ( void )
{
	return new CRenderTexture(
		info.internalFormat, info.width, info.height,
		info.repeatX, info.repeatY, info.type,
		rtInfo.depth, rtInfo.fetchdepth, !rtInfo.fetchcolor, rtInfo.stencil, rtInfo.fetchstencil );
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

		if ( rtInfo.depth != DepthNone )
		{
			if ( rtInfo.fetchdepth )
			{
				cout << " + Attaching depth texture." << endl;
				// Attach the depth texture to depth attachment
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rtInfo.depthtex, 0 );
			}
			else
			{
				// If requested, create a renderbuffer object to store depth info
				glGenRenderbuffers( 1, &rtInfo.depthRBO );
				glBindRenderbuffer( GL_RENDERBUFFER, rtInfo.depthRBO );
				glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum(rtInfo.depth), info.width, info.height );
				glBindRenderbuffer( GL_RENDERBUFFER, 0 );
				// attach the renderbuffer to depth attachment point
				glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtInfo.depthRBO );
			}
		}
		if ( rtInfo.stencil != StencilNone )
		{
			if ( rtInfo.fetchstencil )
			{
				cout << " + Attaching stencil texture." << endl;
				// Attach the stencil texture to stencil attachment
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rtInfo.stenciltex, 0 );
			}
			else
			{
				// If requested, create a renderbuffer object to store stencil info
				glGenRenderbuffers( 1, &rtInfo.stencilRBO );
				glBindRenderbuffer( GL_RENDERBUFFER, rtInfo.stencilRBO );
				glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum(rtInfo.stencil), info.width, info.height );
				glBindRenderbuffer( GL_RENDERBUFFER, 0 );
				// attach the renderbuffer to stencil attachment point
				glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rtInfo.stencilRBO );
			}
		}

		// check FBO status
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if ( status != GL_FRAMEBUFFER_COMPLETE ) {
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
#ifdef _OPENGL4_
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << endl; break;
#endif
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