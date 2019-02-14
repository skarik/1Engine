#include "RrRenderTexture.h"
#include "core/containers/arstring.h"
#include "core/debug/CDebugConsole.h"

//#include "renderer/system/glMainSystem.h"
#include "renderer/exceptions.h"
#include "gpuw/Texture.h"
#include "gpuw/Error.h"

// For debug logging of RT.
//#define RT_DEBUG

//using std::cout;
//using std::endl;

// Definition of buffer stack
//std::stack<unsigned int> RrRenderTexture::buffer_stack;

/*
uint16_t						requestedWidth,
uint16_t						requestedHeight,
core::gfx::tex::arColorFormat	requestedColor	= core::gfx::tex::kColorFormatRGBA8,
core::gfx::tex::arColorFormat	requestedDepth	= core::gfx::tex::kDepthFormatNone,
core::gfx::tex::arColorFormat	requestedStencil= core::gfx::tex::kStencilFormatNone*/

RrRenderTexture::RrRenderTexture ( void )
	: RrTexture("_hx_RT", NULL)
{
	m_rt.create(NULL);
	for (int i = 0; i < 18; ++i) {
		m_attachments[i].attachment	= NULL;
		m_attachments[i].type		= kAttachmentType_None;
	}
}
RrRenderTexture::~RrRenderTexture ( void )
{
	for (RrTexture* texture : rrx_attachments) {
		texture->RemoveReference();
	}
	// Free the target itself
	m_rt.destroy(NULL);
	// The other objects are not owned by the render target.
}

bool RrRenderTexture::Attach ( int slot, RrTexture* texture )
{
	// Ensure the input texture is the right type:
	auto textureClass = texture->ClassType();
	if (textureClass != core::gfx::tex::kTextureClassBase
		&& textureClass != core::gfx::tex::kTextureClass3D
		&& textureClass != core::gfx::tex::kTextureClassCube)
	{
		return false;
	}
	// Ensure the input format matches with the slot:
	auto textureType = texture->GetType();
	if (slot == gpu::kRenderTargetSlotDepth
		&& (textureType <= core::gfx::tex::kDepthFormatNone
		 || textureType >= core::gfx::tex::__DEPTH_FORMAT_MAX))
	{
		return false;
	}
	if (slot == gpu::kRenderTargetSlotStencil
		&& (textureType <= core::gfx::tex::kStencilFormatNone
		 || textureType >= core::gfx::tex::__STENCIL_FORMAT_MAX))
	{
		return false;
	}
	if (slot >= gpu::kRenderTargetSlotColor0
		&& (textureType <= core::gfx::tex::kColorFormatNone
		 || textureType >= core::gfx::tex::__DEPTH_FORMAT_MAX))
	{
		return false;
	}
	// Ensure slot is in our range
	if (slot >= 16) 
	{
		return false;
	}

	// Now, attach the texture to this render target.
	bool success = m_rt.attach(slot, &texture->GetTexture()) == gpu::kError_SUCCESS;

	if (success)
	{
		texture->AddReference();
		rrx_attachments.push_back(texture);

		m_attachments[(slot >= 0) ? slot : 15 - slot].attachment = texture;
		m_attachments[(slot >= 0) ? slot : 15 - slot].type = kAttachmentType_RrTexture;
	}

	return success;
}
bool RrRenderTexture::Attach ( int slot, gpu::Texture* texture )
{
	// Ensure slot is in our range
	if (slot >= 16) 
	{
		return false;
	}

	// Attach the texture to this render target.
	bool success = m_rt.attach(slot, texture) == gpu::kError_SUCCESS;

	if (success)
	{
		tex_attachments.push_back(*texture);

		m_attachments[(slot >= 0) ? slot : 15 - slot].attachment = texture;
		m_attachments[(slot >= 0) ? slot : 15 - slot].type = kAttachmentType_GpuTexture;
	}

	return success;
}
bool RrRenderTexture::Attach ( int slot, gpu::WOFrameAttachment* wof )
{
	// Ensure slot is in our range
	if (slot >= 16) 
	{
		return false;
	}

	// Attach the gpu-only dummy buffer to this render target.
	bool success = m_rt.attach(slot, wof) == gpu::kError_SUCCESS;

	if (success)
	{
		wof_attachments.push_back(*wof);

		m_attachments[(slot >= 0) ? slot : 15 - slot].attachment = wof;
		m_attachments[(slot >= 0) ? slot : 15 - slot].type = kAttachmentType_GpuBuffer;
	}

	return success;
}

bool RrRenderTexture::Validate ( void )
{
	return m_rt.assemble() && m_rt.valid();
}

//
//// == Constructor + Destructor ==
//
//// Explicit Constructor
//RrRenderTexture::RrRenderTexture (
//	unsigned int	requestedWidth,
//	unsigned int	requestedHeight,
//	eWrappingType	repeatX,
//	eWrappingType	repeatY,
//	eColorFormat	requestedColor,
//	eDepthFormat	requestedDepth,
//	eStencilFormat	requestedStencil
//)
//	: RrRenderTexture(
//		requestedWidth, requestedHeight,
//		repeatX, repeatY,
//		requestedColor,
//		RrGpuTexture(0, requestedDepth), requestedDepth != DepthNone,
//		RrGpuTexture(0, requestedStencil), requestedStencil != StencilNone )
//{
//	// Delegated constructor
//}
//
//
//RrRenderTexture::RrRenderTexture (
//	unsigned int	requestedWidth,
//	unsigned int	requestedHeight,
//	eWrappingType	repeatX,
//	eWrappingType	repeatY,
//	eColorFormat	requestedColor,
//	RrGpuTexture		depthRequest,
//	bool			depthFetch,
//	RrGpuTexture		stencilRequest,
//	bool			stencilFetch )
//	: RrTexture ( "_hx_SYSTEM_RENDERTEXTURE" )
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//
//	// Check for NPOT texture support, and modify texture size accordingly.
//	if ( !GL.NPOTsAvailable )
//	{
//		unsigned int nWidth		= 2;
//		unsigned int nHeight	= 2;
//
//		while ( nWidth < requestedWidth )
//		{
//			nWidth *= 2;
//		}
//		while ( nHeight < requestedHeight )
//		{
//			nHeight *= 2;
//		}
//
//		requestedWidth	= nWidth;
//		requestedHeight	= nHeight;
//	}
//
//	// Set the information structure to prepare for reading in
//	info.type			= Texture2D;
//	info.internalFormat	= ColorNone;
//	info.width			= std::min<uint>( requestedWidth, (unsigned)GL.MaxTextureSize );
//	info.height			= std::min<uint>( requestedHeight, (unsigned)GL.MaxTextureSize );
//	info.index			= 0;
//	info.repeatX		= repeatX;
//	info.repeatY		= repeatY;
//
//	rtInfo.fetchcolor	= (requestedColor != ColorNone);
//	rtInfo.depth		= DepthNone;
//	rtInfo.fetchdepth	= false;
//	rtInfo.stencil		= StencilNone;
//	rtInfo.fetchstencil	= false;
//
//	// Initialze data
//	rtInfo.findex		= 0;
//	rtInfo.depthRBO		= 0;
//	rtInfo.stencilRBO	= 0;
//	rtInfo.active		= false;
//
//	// Generate an id
//	arstring256 tempString;
//	sprintf(tempString.data, "__hx_rt_%p_%u_%u", (void*)this, info.width, info.height);
//	rtUniqueSId = tempString;
//
//	// Print out infos
//#	ifdef RT_DEBUG
//	cout << "New render texture: " << rtUniqueSId << endl;
//#	endif
//
//	// Generate the data
//	if ( rtInfo.fetchcolor )
//	{
//#		ifdef RT_DEBUG
//		cout << " + Fetch color enabled." << endl;
//#		endif
//
//		// Create a new color texture and set it up
//		info.index = gpu::TextureAllocate( Texture2D, requestedColor, info.width, info.height );
//		gpu::TextureSampleSettings( Texture2D, info.index, info.repeatX, info.repeatY, 0, SamplingLinear, SamplingLinear );
//		GL.CheckError();
//
//		// Update color format
//		info.internalFormat = requestedColor;
//	}
//	else
//	{
//		info.index = 0;
//	}
//
//	// Do the same if there's a renderbuffer
//	if ( depthFetch )
//	{
//#		ifdef RT_DEBUG
//		cout << " + Fetch depth enabled." << endl;
//#		endif
//
//		if ( depthRequest.texture == 0 )
//		{
//			// Create a new depth texture and set it up
//			rtInfo.depthtex = gpu::TextureAllocate( Texture2D, depthRequest.format, info.width, info.height );
//			gpu::TextureSampleSettings( Texture2D, rtInfo.depthtex, info.repeatX, info.repeatY, 0, SamplingLinear, SamplingLinear );
//			GL.CheckError();
//
//			// Update color format
//			rtInfo.depth = (eDepthFormat)depthRequest.format;
//			rtInfo.depthowned = true;
//		}
//		else
//		{
//			rtInfo.depthtex = depthRequest.texture;
//			rtInfo.depth = (eDepthFormat)depthRequest.format;
//			rtInfo.depthowned = false;
//		}
//	}
//	else
//	{
//		rtInfo.depthtex = 0;
//		if ( depthRequest.texture == 0 )
//		{
//			rtInfo.depth = (eDepthFormat)depthRequest.format;
//			if ( rtInfo.depth != DepthNone )
//				rtInfo.depthRBO = gpu::TextureBufferAllocate(Texture2D, rtInfo.depth, info.width, info.height);
//			rtInfo.depthowned = true;
//		}
//		else
//		{
//			rtInfo.depth = (eDepthFormat)depthRequest.format;
//			rtInfo.depthRBO = depthRequest.texture;
//			rtInfo.depthowned = false;
//		}
//	}
//
//	// Do the same if there's a stencilbuffer
//	if ( stencilFetch )
//	{
//#		ifdef RT_DEBUG
//		cout << " + Fetch stencil enabled." << endl;
//#		endif
//
//		if ( stencilRequest.texture == 0 )
//		{
//			// Create a new stencil texture and set it up
//			rtInfo.stenciltex = gpu::TextureAllocate( Texture2D, stencilRequest.format, info.width, info.height );
//			gpu::TextureSampleSettings( Texture2D, rtInfo.stenciltex, info.repeatX, info.repeatY, 0, SamplingLinear, SamplingLinear );
//			GL.CheckError();
//
//			// Update color format
//			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
//			rtInfo.stencilowned = true;
//		}
//		else
//		{
//			rtInfo.stenciltex = stencilRequest.texture;
//			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
//			rtInfo.stencilowned = false;
//		}
//	}
//	else
//	{
//		rtInfo.stenciltex = 0;
//		if ( stencilRequest.texture == 0 )
//		{
//			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
//			if ( rtInfo.stencil != StencilNone )
//				rtInfo.stencilRBO = gpu::TextureBufferAllocate(Texture2D, rtInfo.stencil, info.width, info.height);
//			rtInfo.stencilowned = true;
//		}
//		else
//		{
//			rtInfo.stencil = (eStencilFormat)stencilRequest.format;
//			rtInfo.stencilRBO = stencilRequest.texture;
//			rtInfo.stencilowned = false;
//		}
//	}
//
//	// And now create the framebuffer
//	GenerateFramebuffer();
//
//	// Make sure the buffer stack is not empty
//	if ( buffer_stack.empty() )
//		buffer_stack.push(0);
//}
//
//// Destructor
//RrRenderTexture::~RrRenderTexture ( void )
//{
//	// Free framebuffer
//	if ( rtInfo.findex )
//		glDeleteFramebuffers( 1, &rtInfo.findex );
//
//	// Free renderbuffers
//	if ( rtInfo.depthRBO && rtInfo.depthowned )
//		glDeleteRenderbuffers( 1, &rtInfo.depthRBO );
//	if ( rtInfo.stencilRBO && rtInfo.stencilowned )
//		glDeleteRenderbuffers( 1, &rtInfo.stencilRBO );
//
//	// Free textures
//	if ( info.index )
//		glDeleteTextures( 1, &info.index );
//	if ( rtInfo.depthtex && rtInfo.depthowned )
//		glDeleteTextures( 1, &rtInfo.depthtex );
//	if ( rtInfo.stenciltex && rtInfo.stencilowned )
//		glDeleteTextures( 1, &rtInfo.stenciltex );
//}
//
//// == Generating Identical-sized Buffers ==
//// Public copy generation
//RrRenderTexture* RrRenderTexture::GenerateCopy ( void )
//{
//	/*return new RrRenderTexture(
//		info.internalFormat, info.width, info.height,
//		info.repeatX, info.repeatY, info.type,
//		rtInfo.depth, rtInfo.fetchdepth, !rtInfo.fetchcolor, rtInfo.stencil, rtInfo.fetchstencil );*/
//	return new RrRenderTexture(
//		info.width, info.height,
//		info.repeatX, info.repeatY,
//		info.internalFormat,
//		RrGpuTexture(0, rtInfo.depth), rtInfo.depthtex != 0,
//		RrGpuTexture(0, rtInfo.stencil), rtInfo.stenciltex != 0 );
//}
//
//// == Texture binding ==
//void RrRenderTexture::BindDepth ( void )
//{
//#ifdef _ENGINE_DEBUG
//	if ( rtInfo.depthtex == 0 )
//		throw renderer::InvalidOperationException();
//#endif
//	glBindTexture( GL_TEXTURE_2D, rtInfo.depthtex );
//}
//void RrRenderTexture::BindStencil ( void )
//{
//#ifdef _ENGINE_DEBUG
//	if ( rtInfo.stenciltex == 0 )
//		throw renderer::InvalidOperationException();
//#endif
//	glBindTexture( GL_TEXTURE_2D, rtInfo.stenciltex );
//}
//
//// == Public Bind and Unbind ==
//// Bind buffer to this guy's FBO
//void RrRenderTexture::BindBuffer ( void )
//{
//	if ( rtInfo.active )
//	{
//		// set rendering destination to FBO
//		glBindFramebuffer( GL_FRAMEBUFFER, rtInfo.findex );
//		if ( rtInfo.fetchcolor ) {
//			GLuint bufferCount [] = {GL_COLOR_ATTACHMENT0};
//			glDrawBuffers( 1, bufferCount );
//		}
//		buffer_stack.push( rtInfo.findex );
//	}
//	else
//	{
//		cout << "Warning: Attempted to bind inactive buffer " << rtUniqueSId << endl;
//		cout << "   Make sure FBO's are supported and your latest OpenGL drivers are installed." << endl;
//	}
//}
//
//// Unbind, meaning bind buffer back to default
//void RrRenderTexture::UnbindBuffer ( void )
//{
//	// Bind buffer back to default
//	if ( !buffer_stack.empty() )
//	{
//		if ( buffer_stack.top() != rtInfo.findex )
//			cout << "ERROR! MISMATCHED BUFFER STACK!" << endl;
//		buffer_stack.pop();
//	}
//	if ( buffer_stack.empty() ) {
//		debug::Console->PrintError( "ERROR! IMBALANCED BUFFER STACK!\n" );
//	}
//	else {
//		glBindFramebuffer( GL_FRAMEBUFFER, buffer_stack.top() ); // TODO: MOVE TO FRAMEBUFFER STACK
//	}
//	//cout << buffer_stack.size() << endl;
//}
//// Global Unbind
//void RrRenderTexture::UnbindBuffer ( char )
//{
//	// Bind buffer back to default
//	if ( !buffer_stack.empty() ) {
//		buffer_stack.pop();
//	}
//	if ( buffer_stack.empty() ) {
//		debug::Console->PrintError( "ERROR! IMBALANCED BUFFER STACK!\n" );
//	}
//	else {
//		glBindFramebuffer( GL_FRAMEBUFFER, buffer_stack.top() );
//	}
//}
//
//// == Private Routines ==
//// Generate framebuffer. This checks if the current framebuffer is valid. If not, it'll create a new framebuffer.
//void RrRenderTexture::GenerateFramebuffer ( void )
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//
//	// Ignore this if there's no FBO support
//	if ( !GL.FBOsAvailable )
//	{
//		rtInfo.active = false;
//		return;
//	}
//	if ( !rtInfo.active )
//	{
//		// create a framebuffer object
//		glGenFramebuffers( 1, &rtInfo.findex );
//		glBindFramebuffer( GL_FRAMEBUFFER, rtInfo.findex );
//
//		if ( rtInfo.fetchcolor )
//		{
//#			ifdef RT_DEBUG
//			cout << " + Attaching color texture." << endl;
//#			endif
//			// attach the texture to FBO color attachment point
//			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, info.index, 0 );
//		}
//		else
//		{
//			// Need to turn color off
//			// NVidia
//			{
//				glDrawBuffer( GL_NONE );
//				glReadBuffer( GL_NONE );
//			}
//			// ATI ?
//			{
//				//glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
//				//glFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, info.index, 0 );
//			}
//		}
//
//		// Bind the render buffer
//		if ( rtInfo.depthRBO != 0 )
//		{
//#			ifdef RT_DEBUG
//			cout << " + Attaching depth RBO." << endl;
//#			endif
//			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtInfo.depthRBO );
//		}
//		else if ( rtInfo.depthtex != 0 )
//		{
//#			ifdef RT_DEBUG
//			cout << " + Attaching depth texture." << endl;
//#			endif
//			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rtInfo.depthtex, 0 );
//		}
//
//		// Bind the stencil buffer
//		if ( rtInfo.stencilRBO != 0 )
//		{
//#			ifdef RT_DEBUG
//			cout << " + Attaching stencil RBO." << endl;
//#			endif
//			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rtInfo.stencilRBO );
//		}
//		else if ( rtInfo.stenciltex != 0 )
//		{
//#			ifdef RT_DEBUG
//			cout << " + Attaching stencil texture." << endl;
//#			endif
//			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, rtInfo.stenciltex, 0 );
//		}
//
//		// check FBO status
//		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
//		if ( status != GL_FRAMEBUFFER_COMPLETE )
//		{
//			rtInfo.active = false;
//
//			switch ( status )
//			{
//			case GL_FRAMEBUFFER_UNDEFINED:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_UNDEFINED" << endl; break;
//			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << endl; break;
//			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl; break;
//			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << endl; break;
//			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << endl; break;
//			case GL_FRAMEBUFFER_UNSUPPORTED:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_UNSUPPORTED" << endl; break;
//			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << endl; break;
//			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
//				cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << endl; break;
//			}
//		}
//		else {
//			rtInfo.active = true;
//		}
//
//		// switch back to window-system-provided framebuffer
//		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
//
//		// Print out success
//		if ( rtInfo.active )
//			cout << " + Render texture " << rtUniqueSId << " is now active." << endl;
//		else
//			cout << " - FAILURE: Render texture " << rtUniqueSId << " failed to activate." << endl;
//	}
//}

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