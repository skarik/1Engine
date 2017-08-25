
// Includes
#include "core/system/io/FileUtils.h"
#include "renderer/state/CRenderState.h"
#include "CTextureCube.h"
#include "CTextureMaster.h"
#include "renderer/system/glMainSystem.h"
#include "core-ext/system/io/Resources.h"

// === Constructor ===
CTextureCube::CTextureCube ( const string &sInIdentifier,
		const string& sInFilenameXPos, const string& sInFilenameXNeg,
		const string& sInFilenameYPos, const string& sInFilenameYNeg,
		const string& sInFilenameZPos, const string& sInFilenameZNeg,
		eTextureType	textureType,
		eColorFormat	format,
		eMipmapGenerationStyle	mipmapGeneration
		) : CTexture( "_hx_SYSTEM_SKIP" )
{
	GL_ACCESS; // Require access to the GL system.

	// Ensure proper format passed in
	if ( textureType != TextureCube ) {
		throw std::exception();
	}

	sFilename = sInIdentifier;
	// Check for system overrides here
	if (( sFilename == "_hx_SYSTEM_FONTLOAD" )||( sFilename == "_hx_SYSTEM_RENDERTEXTURE" )||( sFilename == "_hx_SYSTEM_SKIP" )) {
		return;
	}
	// Look for null texture request (just a fast white sampler)
	if ( sFilename == "null" || sFilename == "Null" || sFilename == "NULL" ) {
		sFilename = "textures/null.jpg";
	}
	// Standardize the filename
	sFilename = IO::FilenameStandardize( sFilename );

	// Set the information structure to prepare for reading in
	info.type			= textureType;
	info.internalFormat	= format;
	info.width			= (unsigned)GL.MaxTextureSize;
	info.height			= (unsigned)GL.MaxTextureSize;
	info.depth			= (unsigned)GL.MaxTextureSize;
	info.index			= 0;
	info.mipmapStyle	= mipmapGeneration;
	
	// Check if the texture has a reference
	const textureEntry_t* pTextureReference = TextureMaster.GetReference( this );
	// Texture doesn't exist yet
	if ( pTextureReference == NULL ) 
	{
		// Send the data to OpenGL
		//info.index = GL.GetNewTexture();
		glGenTextures( 1, &info.index );
		// Bind the texture object
		glBindTexture( GL_TEXTURE_CUBE_MAP, info.index );
		// Set the pack alignment
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		// Copy the data to the texture object
		sFilename = core::Resources::PathTo( sInFilenameXPos );
		LoadImageInfo();
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		delete [] pData;
		pData = NULL;

		sFilename = core::Resources::PathTo( sInFilenameXNeg );
		LoadImageInfo();
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		delete [] pData;
		pData = NULL;

		sFilename = core::Resources::PathTo( sInFilenameYPos );
		LoadImageInfo();
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		delete [] pData;
		pData = NULL;

		sFilename = core::Resources::PathTo( sInFilenameYNeg );
		LoadImageInfo();
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		delete [] pData;
		pData = NULL;

		sFilename = core::Resources::PathTo( sInFilenameZPos );
		LoadImageInfo();
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		delete [] pData;
		pData = NULL;

		sFilename = core::Resources::PathTo( sInFilenameZNeg );
		LoadImageInfo();
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
		delete [] pData;
		pData = NULL;

		// Set blending mode
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Unbind the data
		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
		
		sFilename = sInIdentifier;

		// Add a reference to the data
		TextureMaster.AddReference( this );
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
		info.depth = pTextureReference->info.depth;

		// That's all we need, since everything else has been set already.

		// Add a reference to the data
		TextureMaster.AddReference( this );
	}
}