
// Includes
#include "core/system/io/FileUtils.h"
#include "core/utils/StringUtils.h"
#include "core-ext/system/io/Resources.h"

#include "renderer/state/CRenderState.h"
#include "renderer/resource/CResourceManager.h"
#include "CTexture.h"
#include "CTextureMaster.h"
#include "TextureLoader.h"

#include "renderer/system/glMainSystem.h"

using std::cout;
using std::endl;

// === Constructor ===
CTexture::CTexture ( string sInFilename,
		eTextureType	textureType,
		eInternalFormat	format,
		unsigned int	maxTextureWidth,
		unsigned int	maxTextureHeight,
		eWrappingType	repeatX,
		eWrappingType	repeatY,
		eMipmapGenerationStyle	mipmapGeneration
		)
{
	GL_ACCESS; // Using the glMainSystem accessor

	sFilename = sInFilename;
	// Check for system overrides here
	if (( sFilename == "_hx_SYSTEM_FONTLOAD" )||( sFilename == "_hx_SYSTEM_RENDERTEXTURE" )||( sFilename == "_hx_SYSTEM_SKIP" ))
	{
		return;
	}
	// Look for null texture request (just a fast white sampler)
	if ( sFilename == "null" || sFilename == "Null" || sFilename == "NULL" ) {
		sFilename = "textures/null.jpg";
	}
	// Standardize the filename
	sFilename = IO::FilenameStandardize( sFilename );
	// Look for the valid resource to load
	sFilename = Core::Resources::PathTo( sFilename );
	
	// Null out data
	pData = NULL;
	// Set the information structure to prepare for reading in
	info.type			= textureType;
	info.internalFormat	= format;
	info.width			= std::min<uint>( maxTextureWidth, (unsigned)GL.MaxTextureSize );
	info.height			= std::min<uint>( maxTextureHeight, (unsigned)GL.MaxTextureSize );
	info.index			= 0;
	info.repeatX		= repeatX;
	info.repeatY		= repeatY;
	info.mipmapStyle	= mipmapGeneration;
	// And prepare the state information
	state.level_base	= 0;
	state.level_max		= 0;

	// Check if the texture has a reference
	const glTexture* pTextureReference = TextureMaster.GetReference( this );
	// Texture doesn't exist yet
	if ( pTextureReference == NULL ) 
	{
		// Debug output name of loading texture
		if ( sFilename != sInFilename ) {
			cout << "Loading texture " << sFilename << "(" << sInFilename << ")" << endl;
		}
		else {
			cout << "Loading texture " << sInFilename << endl;
		}

		// Load the image
		//LoadImageInfo();
		tPixel tempData;
		tempData.a = 255;
		tempData.r = 128;
		tempData.g = 128;
		tempData.b = 128;

		// Send the data to OpenGL
		HANDLE context = wglGetCurrentContext();
		//info.index = GL.GetNewTexture();
		glGenTextures( 1, &info.index );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, info.index );
		// Set the pack alignment
		/*glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// Change the lod bias
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f );*/
		// Copy the data to the texture object
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), 1,1, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, &tempData );
		// Generate the mipmaps
		//GenerateMipmap( info.mipmapStyle );
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		// Unbind the data
		glBindTexture( GL_TEXTURE_2D, 0 );

		// Send the data to OpenGL
		/*info.index = GL.GetNewTexture();
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, info.index );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, info.repeatX );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, info.repeatY );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		// Copy the data to the texture object
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, info.width, info.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0 );
		// Bind buffer
		static GLuint pbuf = 0;
		if ( pbuf == 0 ) {
			glGenBuffers( 1, &pbuf );
		}
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbuf );
		// Null existing data
		glBufferData( GL_PIXEL_UNPACK_BUFFER, info.width*info.height*4, NULL, GL_STREAM_DRAW );
		// Map buffer. Returns pointer to buffer memory
		GLubyte* pboMemory = (GLubyte*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
		memcpy( pboMemory, pData, info.width*info.height*4 );
		// Unmaps buffer, indicating we are done writing data to it
		glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
		// Unbind buffer
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );
		// Unbind the texture
		glBindTexture( GL_TEXTURE_2D, 0 );*/

		// Delete the texture data
		//delete [] pData;
		//pData = NULL;

		// Add a reference to the data
		TextureMaster.AddReference( this );

		// Load the actual data in
		if ( !CRenderState::Active->mResourceManager->settings.streamTextures )
		{
			// If streaming disabled, then force load
			CRenderState::Active->mResourceManager->AddResource( this );
			CRenderState::Active->mResourceManager->ForceLoadResource( this );
		}
		else
		{
			// Disable streaming on certain objects
			if ( sFilename.find( "textures/hud" ) != string::npos || sFilename.find( "textures/system" ) != string::npos || sFilename.find( "textures/tats" ) != string::npos )
			{
				CRenderState::Active->mResourceManager->AddResource( this );
				CRenderState::Active->mResourceManager->ForceLoadResource( this );
			}
			// Otherwise, stream the texture
			else
			{
				CRenderState::Active->mResourceManager->FinishAddResource( this );
				CRenderState::Active->mResourceManager->AddResource( this );
			}
		}
	}
	else
	{
		// Get the index of the GL texture
		info.index = pTextureReference->info.index;
		// Get the type of the texture
		info.type = pTextureReference->info.type;
		// Set the size, since that may differ
		info.width = pTextureReference->info.width;
		info.height = pTextureReference->info.height;

		// That's all we need, since everything else has been set (or will be set) already.

		// Add a reference to the data
		TextureMaster.AddReference( this );
		CRenderState::Active->mResourceManager->AddResource( this );
	}
}
// === Destructor ===
CTexture::~CTexture ( void )
{
	// Check for system overrides here
	if (( sFilename == "_hx_SYSTEM_FONTLOAD" )||( sFilename == "_hx_SYSTEM_RENDERTEXTURE" ))
	{
		return;
	}

	//GL.FreeTexture( info.index );
	TextureMaster.RemoveReference( this );
	CRenderState::Active->mResourceManager->RemoveResource( this );
}
// === Bind and Unbind ===
void CTexture::Bind ( void )
{
	if ( info.type == Texture2D ) {
		glDisable( GL_TEXTURE_3D );
		glDisable( GL_TEXTURE_CUBE_MAP );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, info.index );
	}
	else if ( info.type == Texture3D ) {
		glDisable( GL_TEXTURE_CUBE_MAP );
		glEnable( GL_TEXTURE_3D );
		glBindTexture( GL_TEXTURE_3D, info.index );
	}
	else if ( info.type == TextureCube ) {
		glEnable( GL_TEXTURE_CUBE_MAP );
		//cout << info.index << endl;
		glBindTexture( GL_TEXTURE_CUBE_MAP, info.index );
	}
}
void CTexture::Unbind ( void )
{
	if ( info.type == Texture2D ) {
		//glDisable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	else if ( info.type == Texture3D ) {
		//glDisable( GL_TEXTURE_3D );
		glBindTexture( GL_TEXTURE_3D, 0 );
	}
	else if ( info.type == TextureCube ) {
		//glDisable( GL_TEXTURE_CUBE_MAP );
		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	}
}
void CTexture::Unbind ( char a )
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindTexture( GL_TEXTURE_3D, 0 );
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	glBindTexture( GL_TEXTURE_BUFFER, 0 );
}

// === Reloading ===
void CTexture::Reload ( void )
{
	GL_ACCESS;

	// Load the image
	LoadImageInfo();

	// Send the data to OpenGL
	//info.index = GL.GetNewTexture();
	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, info.index );
	// Set the pack alignment
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	// Change the texture repeat
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
	// Change the filtering
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// Copy the data to the texture object
	glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
	// Generate the mipmaps
	GenerateMipmap( info.mipmapStyle );
	// Unbind the data
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Delete the texture data
	delete [] pData;
	pData = NULL;
}

// === Mipmap Generation ===
void CTexture::GenerateMipmap ( eMipmapGenerationStyle generationStyle )
{
	GL_ACCESS;

	switch ( generationStyle ) {
	case MipmapNone:
		// Nothing to do.
		break;
	case MipmapNormal:
		// Change filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		// Generate the mipmaps
		glGenerateMipmap( GL_TEXTURE_2D );
		break;
	case MipmapNearest:
		// Change filtering
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		// Change mipmap number
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
		// Upload mipmaps
		// Decrease data resolution
		for ( uint y = 0; y < info.height/2; ++y ) {
			for ( uint x = 0; x < info.width/2; ++x ) {
				pData[x+(y*(info.width/2))] = pData[(x+(y*(info.width)))*2];
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 1, GL.Enum(info.internalFormat), info.width/2, info.height/2, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		for ( uint y = 0; y < info.height/4; ++y ) {
			for ( uint x = 0; x < info.width/4; ++x ) {
				pData[x+(y*(info.width/4))] = pData[(x+(y*(info.width/2)))*2];
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 2, GL.Enum(info.internalFormat), info.width/4, info.height/4, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		for ( uint y = 0; y < info.height/8; ++y ) {
			for ( uint x = 0; x < info.width/8; ++x ) {
				pData[x+(y*(info.width/8))] = pData[(x+(y*(info.width/4)))*2];
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 3, GL.Enum(info.internalFormat), info.width/8, info.height/8, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		for ( uint y = 0; y < info.height/16; ++y ) {
			for ( uint x = 0; x < info.width/16; ++x ) {
				pData[x+(y*(info.width/16))] = pData[(x+(y*(info.width/8)))*2];
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 4, GL.Enum(info.internalFormat), info.width/16, info.height/16, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		for ( uint y = 0; y < info.height/32; ++y ) {
			for ( uint x = 0; x < info.width/32; ++x ) {
				pData[x+(y*(info.width/32))] = pData[(x+(y*(info.width/16)))*2];
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 5, GL.Enum(info.internalFormat), info.width/32, info.height/32, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		break;
	}
}

// === Loader ===
void CTexture::LoadImageInfo ( void )
{
	Textures::timgInfo t_info;
	pData = Textures::LoadRawImageData( sFilename, t_info );
	this->info.width			= t_info.width;
	this->info.height			= t_info.height;
	this->info.internalFormat	= (eInternalFormat)t_info.internalFormat;
}