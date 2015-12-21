
#include "CMRTTexture.h"
#include "core/debug/CDebugConsole.h"
#include "renderer/system/glMainSystem.h"

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
		eInternalFormat	format			,
		unsigned int	maxTextureWidth	,
		unsigned int	maxTextureHeight,
		eWrappingType	repeatX			,
		eWrappingType	repeatY			,
		eDepthFormat	depthType		,
		bool			depthFetch		,
		eStencilFormat	stencilType		,
		bool			stencilFetch	,
		uint			colorAttachments
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
	info.type			= Texture2D;
	info.internalFormat	= format;
	info.width			= std::min<uint>( maxTextureWidth, (unsigned)GL.MaxTextureSize );
	info.height			= std::min<uint>( maxTextureHeight, (unsigned)GL.MaxTextureSize );
	info.index			= 0;
	info.repeatX		= repeatX;
	info.repeatY		= repeatY;

	info.mipmapStyle	= MipmapNone;
	info.filter			= SamplingLinear;
	
	rtInfo.fetchcolor	= (colorAttachments!=0);
	rtInfo.depth		= depthType;
	rtInfo.fetchdepth	= depthFetch;
	rtInfo.stencil		= stencilType;
	rtInfo.fetchstencil	= stencilFetch;

	// Initialze data
	rtInfo.findex		= 0;
	rtInfo.depthRBO		= 0;
	rtInfo.stencilRBO	= 0;
	rtInfo.active		= false;

	mrtInfo.colorAttachments	= colorAttachments;
	mrtInfo.depthAttachment		= -1;
	mrtInfo.stencilAttachment	= -1;

	// Generate an id
	stringstream tempstream;
	tempstream << "__hx_rt_M_" << (void*)this << "_" << info.width << "_" << info.height; 
	rtUniqueSId = string( tempstream.str() );

	// Print out infos
	cout << "New render texture: " << rtUniqueSId << endl;

	// Generate the textures for the colors
	for ( int i = 0; i < mrtInfo.colorAttachments; ++i )
	{
		cout << " + Fetch color " << i << " enabled." << endl;
		// Send the data to OpenGL
		//mrtInfo.tx_indices[i] = GL.GetNewTexture();
		glGenTextures( 1, &(mrtInfo.tx_indices[i]) );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, mrtInfo.tx_indices[i] );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
		// Turn on automatic mipmap generation
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		// Copy the data to the texture object
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		// Unbind the data
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int8_t m_attachmentIndex = mrtInfo.colorAttachments;

	// Do the same if there's a renderbuffer
	if (( rtInfo.depth != DepthNone )&&( rtInfo.fetchdepth ))
	{
		cout << " + Fetch depth enabled." << endl;
		mrtInfo.depthAttachment = m_attachmentIndex;
		m_attachmentIndex += 1;
		// Send the data to OpenGL
		glGenTextures( 1, &(mrtInfo.tx_indices[mrtInfo.depthAttachment]) );
		//mrtInfo.tx_indices[mrtInfo.depthAttachment] = GL.GetNewTexture();
		rtInfo.depthtex = mrtInfo.tx_indices[mrtInfo.depthAttachment];
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, mrtInfo.tx_indices[mrtInfo.depthAttachment] );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
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
		mrtInfo.depthAttachment = -1;
	}

	// Do the same if there's a stencilbuffer
	if (( rtInfo.stencil != StencilNone )&&( rtInfo.fetchstencil ))
	{
		cout << " + Fetch stencil enabled." << endl;
		mrtInfo.stencilAttachment = m_attachmentIndex;
		m_attachmentIndex += 1;
		// Send the data to OpenGL
		glGenTextures( 1, &(mrtInfo.tx_indices[mrtInfo.stencilAttachment]) );
		//mrtInfo.tx_indices[mrtInfo.stencilAttachment] = GL.GetNewTexture();
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, mrtInfo.tx_indices[mrtInfo.stencilAttachment] );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
		// Turn on automatic mipmap generation
		GL.CheckError();
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		// Copy the data to the texture object
		GL.CheckError();
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(rtInfo.stencil), info.width, info.height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0 );
		GL.CheckError();
		// Unbind the data
		glBindTexture(GL_TEXTURE_2D, 0);
		GL.CheckError();
	}
	else
	{
		rtInfo.stenciltex = 0;
		rtInfo.stencilRBO = 0;
		mrtInfo.stencilAttachment = -1;
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
	int8_t bufferCount = mrtInfo.colorAttachments;
	if ( mrtInfo.depthAttachment >= 0 ) {
		bufferCount += 1;
	}
	if ( mrtInfo.stencilAttachment >= 0 ) {
		bufferCount += 1;
	}
	for ( int8_t i = 0; i < bufferCount; ++i )
	{
		//GL.FreeTexture( mrtInfo.tx_indices[i] );
		glDeleteTextures( 1, &(mrtInfo.tx_indices[i]) );
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
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, mrtInfo.tx_indices[i], 0 );
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
				// If requested depth, but no depth texture, create a renderbuffer object to store depth info
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
				// If requested stencil, but no stencil texture, create a renderbuffer object to store stencil info
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
#			ifdef _OPENGL4_
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << endl; break;
#			endif
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