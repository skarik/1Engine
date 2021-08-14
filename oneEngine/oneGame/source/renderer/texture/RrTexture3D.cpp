#include "core/system/io/FileUtils.h"
#include "core/utils/string.h"
#include "core/debug/console.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "renderer/state/RrRenderer.h"
#include "RrTexture3D.h"
#include "RrTextureMaster.h"

// TODO: Load3D.
// TODO: LoadAtlas. It needs a unique streamed behavior where it loads lower mip levels of all layers, then works its way up.
//                  The atlas is loaded as a 2D texture array in order to ensure proper mipmap behavior.

//	CreateUnitialized ( name ) : Creates an uninitialized texture object.
// Can be used for procedural textures, with Upload(...) later.
RrTexture3D*
RrTexture3D::CreateUnitialized ( const char* name )
{
	arstring256 resource_str_id (name);

	// We need to create a new texture:
	RrTexture3D* texture = new RrTexture3D(resource_str_id, NULL);

	// We don't add it to the resource system yet. For now, we just return it.

	return texture;
}

RrTexture3D::RrTexture3D (
	const char* s_resourceId,
	const char* s_resourcePath
	)
	: RrTexture(s_resourceId, s_resourcePath)
{
	// All handled by RrTexture.
}

//	Upload(...) : Upload data to the texture, initializing it.
// Use for procedural textures.
void
RrTexture3D::Upload (
	bool							streamed,
	void*							data,
	uint16_t						width,
	uint16_t						height,
	uint16_t						depth,
	core::gfx::tex::arColorFormat	format,
	core::gfx::tex::arWrappingType	repeatX,
	core::gfx::tex::arWrappingType	repeatY,
	core::gfx::tex::arWrappingType	repeatZ,
	core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration,
	core::gfx::tex::arSamplingFilter defaultSamplerFilter
)
{
	auto resm = core::ArResourceManager::Active();

	if (!procedural)
	{
		debug::Console->PrintError("RrTexture3D.cpp : trying to upload on nonproc texture");
		return;
	}

	if (upload_request == NULL) {
		upload_request = new rrTextureUploadInfo();
	}
	upload_request->type = core::gfx::tex::kTextureType3D;
	upload_request->data = data;
	upload_request->width = width;
	upload_request->height = height;
	upload_request->depth = depth;
	upload_request->format = format;

	// Update the info with the input params
	info.repeatX = repeatX;
	info.repeatY = repeatY;
	info.repeatZ = repeatZ;
	info.mipmapStyle = mipmapGeneration;
	info.filter = defaultSamplerFilter;

	// Set streamed setting before adding self to resource system.
	this->streamed = streamed;
	// Add self to the resource system:
	if (resm->Contains(this) == false)
		resm->Add(this);
	else // Or force an update:
		Reload();
}

RrTexture3D::~RrTexture3D ( void )
{
	// All handled by RrTexture.
}

//// Includes
////#include "RrRenderer.h"
//#include "renderer/system/glSystem.h"
//#include "RrTexture3D.h"
//#include "RrTextureMaster.h"
//
//#include "core/system/io/FileUtils.h"
//#include "core-ext/system/io/Resources.h"
//
//// === Constructor ===
//RrTexture3D::RrTexture3D ( string sInFilename,
//		eTextureType	textureType,
//		eColorFormat	format,
//		unsigned int	atlasSizeX,
//		unsigned int	atlasSizeY,
//		eWrappingType	repeatX,
//		eWrappingType	repeatY,
//		eWrappingType	repeatZ,
//		eMipmapGenerationStyle	mipmapGeneration
//		) : RrTexture( "_hx_SYSTEM_SKIP" )
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//
//	sFilename = sInFilename;
//	// Check for system overrides here
//	if (( sFilename == "_hx_SYSTEM_FONTLOAD" )||( sFilename == "_hx_SYSTEM_RENDERTEXTURE" )) {
//		return;
//	}
//	// Look for null texture request (just a fast white sampler)
//	if ( sFilename == "null" || sFilename == "Null" || sFilename == "NULL" ) {
//		sFilename = "textures/null.jpg";
//	}
//	// Standardize the filename
//	sFilename = IO::FilenameStandardize( sFilename );
//	// Look for the valid resource to load
//	sFilename = core::Resources::PathTo( sFilename );
//
//	// Set the information structure to prepare for reading in
//	info.type			= textureType;
//	info.internalFormat	= format;
//	info.width			= (unsigned)GL.MaxTextureSize;
//	info.height			= (unsigned)GL.MaxTextureSize;
//	info.depth			= (unsigned)GL.MaxTextureSize;
//	info.index			= 0;
//	info.repeatX		= repeatX;
//	info.repeatY		= repeatY;
//	info.repeatZ		= repeatZ;
//	info.mipmapStyle	= mipmapGeneration;
//	
//	// Check if the texture has a reference
//	const textureEntry_t* pTextureReference = TextureMaster.GetReference( this );
//	// Texture doesn't exist yet
//	if ( pTextureReference == NULL ) 
//	{
//		// Load the image
//		Load3DImageInfoFromAtlas( atlasSizeX, atlasSizeY );
//
//		// Send the data to OpenGL
//		//info.index = GL.GetNewTexture();
//		glGenTextures( 1, &info.index );
//		// Bind the texture object
//		glBindTexture( GL_TEXTURE_3D, info.index );
//		// Set the pack alignment
//		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
//		// Change the texture repeat
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL.Enum(info.repeatZ) );
//		// Change the filtering
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//		// Change the lod bias
//		glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_LOD_BIAS, -0.5f );
//		// Copy the data to the texture object
//		glTexImage3D( GL_TEXTURE_3D, 0, info.internalFormat, info.width, info.height, info.depth, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		// Generate the mipmaps
//		GenerateMipmap3D( info.mipmapStyle );
//		// Unbind the data
//		glBindTexture( GL_TEXTURE_3D, 0 );
//
//		// Delete the texture data
//		delete [] pData;
//		pData = NULL;
//
//		// Add a reference to the data
//		TextureMaster.AddReference( this );
//	}
//	else
//	{
//		// Get the index of the GL texture
//		info.index = pTextureReference->info.index;
//		// Get the type of the texture
//		info.type = pTextureReference->info.type;
//		// Set the size, since that may differ
//		info.width = pTextureReference->info.width;
//		info.height = pTextureReference->info.height;
//		info.depth = pTextureReference->info.depth;
//
//		// That's all we need, since everything else has been set already.
//
//		// Add a reference to the data
//		TextureMaster.AddReference( this );
//	}
//}
//
//// Converting 2D atlas into a 3D texture
//void RrTexture3D::Load3DImageInfoFromAtlas ( const unsigned int atlasSizeX, const unsigned int atlasSizeY )
//{
//	// Load the image normally
//	LoadImageInfo();
//
//	// Set new proper dimensions
//	info.width /= atlasSizeX;
//	info.height /= atlasSizeY;
//	info.depth = atlasSizeX*atlasSizeY;
//	
//	// Rearrange the array by swapping pixel data
//	tPixel* pOldData = pData;
//	pData = new tPixel [info.width*info.height*info.depth];
//	for ( uint x = 0; x < info.width; ++x )
//	{
//		for ( uint y = 0; y < info.height; ++y )
//		{
//			//pOldData[x+y*info.width];
//			for ( uint z = 0; z < info.depth; ++z )
//			{
//				pData[x+(y*info.width)+(z*info.width*info.height)] =
//					pOldData[
//						x + ( (z%atlasSizeX)*info.width ) +
//						( (y+((z/atlasSizeX)*info.height) )*info.width*atlasSizeX)
//						];
//			}
//		}
//	}
//	delete [] pOldData;
//	pOldData = NULL;
//}
//
//
////=========================================//
//// === Manual Upload ===
//void RrTexture3D::Upload (
//	pixel_t* data,
//	uint width,
//	uint height,
//	uint depth,
//	eWrappingType	repeatX,
//	eWrappingType	repeatY,
//	eWrappingType	repeatZ,
//	eMipmapGenerationStyle	mipmapGeneration,
//	eSamplingFilter	filter
//)
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//
//			   // Null out data
//	pData = NULL;
//	// Set the information structure to prepare for reading in
//	info.type			= Texture3D;
//	info.internalFormat	= RGBA8;
//	info.width			= width;
//	info.height			= height;
//	info.depth			= depth;
//	info.repeatX		= repeatX;
//	info.repeatY		= repeatY;
//	info.repeatZ		= repeatZ;
//	info.mipmapStyle	= mipmapGeneration;
//	info.filter			= filter;
//	// And prepare the state information
//	state.level_base	= 0;
//	state.level_max		= 0;
//
//	if (info.index == 0)
//	{
//		// Create texture
//		glGenTextures( 1, &info.index );
//	}
//
//	// Bind the texture object
//	glBindTexture( GL_TEXTURE_3D, info.index );
//	// Set the pack alignment
//	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
//	// Change the texture repeat
//	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
//	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
//	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL.Enum(info.repeatZ) );
//	// Change the filtering
//	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
//	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
//	// Copy the data to the texture object
//	glTexImage3D( GL_TEXTURE_3D, 0, GL.Enum(info.internalFormat), width, height, depth, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data );
//	// Generate the mipmaps
//	GenerateMipmap( info.mipmapStyle );
//	// Unbind the data
//	glBindTexture( GL_TEXTURE_3D, 0 );
//}
//
//// === Mipmap Generation ===
//void RrTexture3D::GenerateMipmap3D ( eMipmapGenerationStyle generationStyle )
//{
//	switch ( generationStyle ) {
//	case MipmapNone:
//		// Nothing to do.
//		break;
//	case MipmapNormal:
//		// Change filtering
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//		// Generate the mipmaps
//		glGenerateMipmap( GL_TEXTURE_3D );
//		break;
//	case MipmapNearest:
//		// Change filtering
//		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//		/*glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
//		// Change mipmap number
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
//		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 5);
//		// Upload mipmaps
//		// Decrease data resolution
//		for ( uint y = 0; y < info.height/2; ++y ) {
//			for ( uint x = 0; x < info.width/2; ++x ) {
//				pData[x+(y*(info.width/2))] = pData[(x+(y*(info.width)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 1, info.internalFormat, info.width/2, info.height/2, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/4; ++y ) {
//			for ( uint x = 0; x < info.width/4; ++x ) {
//				pData[x+(y*(info.width/4))] = pData[(x+(y*(info.width/2)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 2, info.internalFormat, info.width/4, info.height/4, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/8; ++y ) {
//			for ( uint x = 0; x < info.width/8; ++x ) {
//				pData[x+(y*(info.width/8))] = pData[(x+(y*(info.width/4)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 3, info.internalFormat, info.width/8, info.height/8, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/16; ++y ) {
//			for ( uint x = 0; x < info.width/16; ++x ) {
//				pData[x+(y*(info.width/16))] = pData[(x+(y*(info.width/8)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 4, info.internalFormat, info.width/16, info.height/16, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/32; ++y ) {
//			for ( uint x = 0; x < info.width/32; ++x ) {
//				pData[x+(y*(info.width/32))] = pData[(x+(y*(info.width/16)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 5, info.internalFormat, info.width/32, info.height/32, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		break;*/
//		break;
//	}
//}