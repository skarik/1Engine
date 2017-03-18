
#include "CMRTTexture.h"
#include "core/debug/CDebugConsole.h"
#include "renderer/system/glMainSystem.h"

#include "renderer/gpuw/Textures.h"

// Stringstream for unique string id generation
#include <sstream>

using std::stringstream;
using std::cout;
using std::endl;

void printFramebufferLimits()
{
	int res;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
	printf("Max Color Attachments: %d\n", res);
	/*
	glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &res);
	printf("Max Framebuffer Width: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &res);
	printf("Max Framebuffer Height: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &res);
	printf("Max Framebuffer Samples: %d\n", res);

	glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &res);
	printf("Max Framebuffer Layers: %d\n", res);*/
	return;
}

CMRTTexture::CMRTTexture ( 
	unsigned int	requestedWidth,
	unsigned int	requestedHeight,
	eWrappingType	repeatX,
	eWrappingType	repeatY,
	glTexture*		rgbRequestList,
	const uint		rgbRequestListSize,
	glTexture*		depthRequest,
	bool			depthFetch,
	glTexture*		stencilRequest,
	bool			stencilFetch
)
	: CRenderTexture ( "_hx_SYSTEM_RENDERTEXTURE" )
{
	GL_ACCESS;

	printFramebufferLimits();

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

	info.mipmapStyle	= MipmapNone;
	info.filter			= SamplingLinear;

	rtInfo.fetchcolor	= rgbRequestListSize != 0;
	rtInfo.depth		= DepthNone;
	rtInfo.fetchdepth	= depthFetch;
	rtInfo.stencil		= StencilNone;
	rtInfo.fetchstencil	= stencilFetch;

	// Initialze data
	rtInfo.findex		= 0;
	rtInfo.depthRBO		= 0;
	rtInfo.stencilRBO	= 0;
	rtInfo.active		= false;

	mrtInfo.attachments			= 0;
	mrtInfo.colorAttachments	= 0;
	mrtInfo.depthAttachment		= 0;
	mrtInfo.stencilAttachment	= 0;

	// Generate an id
	stringstream tempstream;
	tempstream << "__hx_rt_M_" << (void*)this << "_" << info.width << "_" << info.height; 
	rtUniqueSId = string( tempstream.str() );

	// Print out infos
	cout << "New render texture: " << rtUniqueSId << endl;

	// Generate the texture information for the colors
	for ( uint i = 0; i < rgbRequestListSize; ++i )
	{
		if ( rgbRequestList[i].texture == 0 )
		{
			mrtInfo.texture_formats[mrtInfo.attachments] = rgbRequestList[i].format;
			mrtInfo.texture_owned[mrtInfo.attachments] = true;

			mrtInfo.texture[mrtInfo.attachments] = GPU::TextureAllocate(
				Texture2D,
				mrtInfo.texture_formats[mrtInfo.attachments], info.width, info.height );
			GPU::TextureSampleSettings(
				Texture2D,
				mrtInfo.texture[mrtInfo.attachments],
				info.repeatX, info.repeatY, 0, info.filter, info.filter );

			GL.CheckError();
		}
		else
		{
			mrtInfo.texture[mrtInfo.attachments] = rgbRequestList[i].texture;
			mrtInfo.texture_formats[mrtInfo.attachments] = rgbRequestList[i].format;
			mrtInfo.texture_owned[mrtInfo.attachments] = false;
		}

		mrtInfo.attachments += 1;
	}
	mrtInfo.colorAttachments = mrtInfo.attachments;

	// Generate the texture information for the depth
	if ( depthFetch )
	{
		if ( depthRequest->texture == 0 )
		{
			mrtInfo.texture_formats[mrtInfo.attachments] = depthRequest->format;
			mrtInfo.texture_owned[mrtInfo.attachments] = true;

			mrtInfo.texture[mrtInfo.attachments] = GPU::TextureAllocate(
				Texture2D,
				mrtInfo.texture_formats[mrtInfo.attachments], info.width, info.height );
			GPU::TextureSampleSettings(
				Texture2D,
				mrtInfo.texture[mrtInfo.attachments],
				info.repeatX, info.repeatY, 0, info.filter, info.filter );

			GL.CheckError();
		}
		else
		{
			mrtInfo.texture[mrtInfo.attachments] = depthRequest->texture;
			mrtInfo.texture_formats[mrtInfo.attachments] = depthRequest->format;
			mrtInfo.texture_owned[mrtInfo.attachments] = false;
		}
		rtInfo.depth = (eDepthFormat)depthRequest->format;
		rtInfo.depthtex = mrtInfo.texture[mrtInfo.attachments];

		mrtInfo.depthAttachment = mrtInfo.attachments;
		mrtInfo.attachments += 1;
	}
	else if ( depthRequest != NULL )
	{
		rtInfo.depthtex = 0;
		if ( depthRequest->texture == 0 )
		{
			rtInfo.depth = (eDepthFormat)depthRequest->format;
			if ( rtInfo.depth != DepthNone )
				rtInfo.depthRBO = GPU::TextureBufferAllocate(Texture2D, rtInfo.depth, info.width, info.height);
			rtInfo.depthowned = true;
		}
		else
		{
			rtInfo.depth = (eDepthFormat)depthRequest->format;
			rtInfo.depthRBO = depthRequest->texture;
			rtInfo.depthowned = false;
		}
	}

	// Generate the texture information for the stencil
	if ( stencilFetch )
	{
		if ( stencilRequest->texture == 0 )
		{
			mrtInfo.texture_formats[mrtInfo.attachments] = stencilRequest->format;
			mrtInfo.texture_owned[mrtInfo.attachments] = true;

			mrtInfo.texture[mrtInfo.attachments] = GPU::TextureAllocate(
				Texture2D,
				mrtInfo.texture_formats[mrtInfo.attachments], info.width, info.height );
			GPU::TextureSampleSettings(
				Texture2D,
				mrtInfo.texture[mrtInfo.attachments],
				info.repeatX, info.repeatY, 0, info.filter, info.filter );

			GL.CheckError();
		}
		else
		{
			mrtInfo.texture[mrtInfo.attachments] = stencilRequest->texture;
			mrtInfo.texture_formats[mrtInfo.attachments] = stencilRequest->format;
			mrtInfo.texture_owned[mrtInfo.attachments] = false;
		}
		rtInfo.stencil = (eStencilFormat)stencilRequest->format;
		rtInfo.stenciltex = mrtInfo.texture[mrtInfo.attachments];

		mrtInfo.stencilAttachment = mrtInfo.attachments;
		mrtInfo.attachments += 1;
	}
	else if ( stencilRequest != NULL )
	{
		rtInfo.stenciltex = 0;
		if ( stencilRequest->texture == 0 )
		{
			rtInfo.stencil = (eStencilFormat)stencilRequest->format;
			if ( rtInfo.stencil != StencilNone )
				rtInfo.stencilRBO = GPU::TextureBufferAllocate(Texture2D, rtInfo.stencil, info.width, info.height);
			rtInfo.stencilowned = true;
		}
		else
		{
			rtInfo.stencil = (eStencilFormat)stencilRequest->format;
			rtInfo.stencilRBO = stencilRequest->texture;
			rtInfo.stencilowned = false;
		}
	}

	// And now create the framebuffer
	GenerateFramebuffer();
	GL.CheckError();

	// Make sure the buffer stack is not empty
	if ( buffer_stack.empty() )
		buffer_stack.push(0);
}


CMRTTexture::~CMRTTexture ( void )
{
	// Don't let CRenderTexture delete the textures
	info.index = 0;
	rtInfo.depthtex = 0;
	rtInfo.stenciltex = 0;

	// Delete the owned textures
	for ( int8_t i = 0; i < mrtInfo.attachments; ++i )
	{
		if ( mrtInfo.texture_owned[i] )
		{
			glDeleteTextures( 1, &(mrtInfo.texture[i]) );
		}
	}
}

// == Public Bind and Unbind ==
// Bind buffer to this guy's FBO
void CMRTTexture::BindBuffer ( void )
{
	if ( rtInfo.active )
	{
		// set rendering destinations to target color attachments on the buffer
		GLenum buffers [16];
		GLuint bufferCount = mrtInfo.colorAttachments;
		for ( uint8_t i = 0; i < bufferCount; ++i ) {
			buffers[i] = GL_COLOR_ATTACHMENT0+i;
		}
		buffers[0] = GL_COLOR_ATTACHMENT0;
		glBindFramebuffer( GL_FRAMEBUFFER, rtInfo.findex );
		glDrawBuffers( bufferCount, buffers );
		/*if ( rtInfo.fetchcolor ) {
			GLuint bufferCount [] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers( 1, bufferCount );
		}*/
		buffer_stack.push( rtInfo.findex );
	}
	else
	{
		cout << "Warning: Attempted to bind inactive buffer " << rtUniqueSId << endl;
		cout << "   Make sure FBO's are supported and your latest OpenGL drivers are installed." << endl;
	}
}

// Unbind, meaning bind buffer back to default
void CMRTTexture::UnbindBuffer ( void )
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

		/*if ( buffer_stack.top() != 0 ) {
			GLuint bufferCount [] = {GL_COLOR_ATTACHMENT0};//{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers( 1, bufferCount );
		}*/
		/*else {
			GLuint bufferCount [] = {GL_FRONT_LEFT};//{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers( 1, bufferCount );
		}*/
	}
	//cout << buffer_stack.size() << endl;
}

// == Private Routines ==
// Generate framebuffer. This checks if the current framebuffer is valid. If not, it'll create a new framebuffer.
void CMRTTexture::GenerateFramebuffer ( void )
{	GL_ACCESS

	// Ignore this if there's no FBO support
	if ( !GL.FBOsAvailable )
	{
		rtInfo.active = false;
		cout << " - FBOs not available. Could not generate framebuffers. " << endl;
		return;
	}
	if ( !rtInfo.active )
	{
		// create a framebuffer object
		glGenFramebuffers( 1, &rtInfo.findex );
		glBindFramebuffer( GL_FRAMEBUFFER, rtInfo.findex );

		if ( !rtInfo.fetchcolor )
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
		// Otherwise, then we can loop through the render targets
		for ( int8_t i = 0; i < mrtInfo.colorAttachments; ++i )
		{
			cout << " + Attaching color texture " << (int)i << endl;
			// attach the texture to FBO color attachment point
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, mrtInfo.texture[i], 0 );
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
		// And we're done!
	}
}


//=========================================//
// === Setters ===
void CMRTTexture::SetFilter ( eSamplingFilter filter )
{
	GL_ACCESS;

	// Update value
	info.filter = filter;

	for ( int8_t i = 0; i < mrtInfo.attachments; ++i )
	{
		if ( mrtInfo.texture_owned[i] )
		{
			// Bind the texture object
			glBindTexture( GL_TEXTURE_2D, mrtInfo.texture[i] );
			// Change the filtering
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
		}
	}

	// Unbind the data
	glBindTexture( GL_TEXTURE_2D, 0 );
}
