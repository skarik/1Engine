#include "core/system/io/FileUtils.h"
#include "core/utils/string.h"
#include "core/debug/console.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "renderer/state/RrRenderer.h"
#include "RrTexture.h"
#include "RrTextureMaster.h"

//	Load ( filename ) : Loads a texture from the disk.
// May return a previously loaded instance of the texture.
RrTexture*
RrTexture::Load ( const char* resource_name )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filename:
	arstring256 resource_str_id (resource_name);
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the texture in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrTexture, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrTexture* existingTexture = (RrTexture*)existingResource;
		existingTexture->AddReference(); // TODO: the docs say this doesn't happen. Evaluate and remove this.
		return existingTexture;
	}

	// TODO: Cache the lookup misses, return NULL.
	// Check if the file exists. Required for 2D to fail gracefully.
	arstring256 resource_str_bpd = (std::string(resource_str_id) + ".bpd").c_str();
	if (!core::Resources::Exists(resource_name) && !core::Resources::Exists(resource_str_bpd))
	{
		ARCORE_ERROR("Invalid file \"%s\" passed in.", resource_name);
		return NULL;
	}

	// We need to create a new texture:
	RrTexture* texture = new RrTexture(resource_str_id, resource_name);

	// Add it to the resource system:
	resm->Add(texture);

	return texture;
}
//	CreateUnitialized ( name ) : Creates an uninitialized texture object.
// Can be used for procedural textures, with Upload(...) later.
RrTexture*
RrTexture::CreateUnitialized ( const char* name )
{
	arstring256 resource_str_id (name);
	core::utils::string::ToResourceName(resource_str_id);

	// We need to create a new texture:
	RrTexture* texture = new RrTexture(resource_str_id, NULL);
	
	// We don't add it to the resource system yet. For now, we just return it.

	return texture;
}
//	Find ( name ) : Finds a texture with the given resource handle.
// Can be used to locate previously created textures, especially procedural ones.
RrTexture*
RrTexture::Find ( const char* name )
{
	auto resm = core::ArResourceManager::Active();

	// Find the texture in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrTexture, name);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrTexture* existingTexture = (RrTexture*)existingResource;
		return existingTexture;
	}

	// Otherwise, didn't find anything.
	return NULL;
}

RrTexture::RrTexture (
	const char* s_resourceId,
	const char* s_resourcePath
	)
	: arBaseObject(),
	m_texture()
{
	RemoveReference(); // Start with 0 reference count.

	resourceName = s_resourceId;
	// Reset load state:
	StreamingReset();
	
	if (s_resourcePath == NULL)
	{
		procedural = true;
		streamed = false;
		upload_request = NULL;
	}
	else
	{
		procedural = false;
		upload_request = NULL;

		// Update stream state based on resource path
		if (strstr(s_resourcePath, "textures/hud") != NULL
			|| strstr(s_resourcePath, "textures/system") != NULL
			|| strstr(s_resourcePath, "textures/tats") != NULL)
		{
			streamed = false;
		}
		else
		{
			streamed = true; // Overriden by resm->m_settings.streamTextures
		}

		// Set resource name
		resourceFilename = s_resourcePath;
	}
}

//	Upload(...) : Upload data to the texture, initializing it.
// Use for procedural textures.
void
RrTexture::Upload (
	bool							streamed,
	void*							data,
	uint16_t						width,
	uint16_t						height,
	core::gfx::tex::arColorFormat	format,
	core::gfx::tex::arWrappingType	repeatX,
	core::gfx::tex::arWrappingType	repeatY,
	core::gfx::tex::arMipmapGenerationStyle	mipmapGeneration,
	core::gfx::tex::arSamplingFilter defaultSamplerFilter
)
{
	auto resm = core::ArResourceManager::Active();

	if (!procedural)
	{
		debug::Console->PrintError("RrTexture.cpp : trying to upload on nonproc texture");
		return;
	}

	if (upload_request == NULL) {
		upload_request = new rrTextureUploadInfo();
	}
	upload_request->type = core::gfx::tex::kTextureType2D;
	upload_request->data = data;
	upload_request->width = width;
	upload_request->height = height;
	upload_request->depth = 1;
	upload_request->format = format;

	// Update the info with the input params
	info.repeatX = repeatX;
	info.repeatY = repeatY;
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

RrTexture::~RrTexture ( void )
{
	m_texture.free();
}

//
//// === Constructor ===
//RrTexture::RrTexture ( string sInFilename,
//		eTextureType	textureType,
//		eColorFormat	format,
//		unsigned int	maxTextureWidth,
//		unsigned int	maxTextureHeight,
//		eWrappingType	repeatX,
//		eWrappingType	repeatY,
//		eMipmapGenerationStyle	mipmapGeneration,
//		eSamplingFilter	filter
//		)
//		: arBaseObject()
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//
//	sFilename = sInFilename;
//	// Check for system overrides here
//	if ( sFilename == "" || sFilename == "_hx_SYSTEM_FONTLOAD" || sFilename == "_hx_SYSTEM_RENDERTEXTURE" || sFilename == "_hx_SYSTEM_SKIP" )
//	{
//		info.index = 0;
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
//#ifndef _ENGINE_DEBUG
//	throw core::NotYetImplementedException();
//#endif
//
//	// Null out data
//	pData = NULL;
//	// Set the information structure to prepare for reading in
//	info.type			= textureType;
//	info.internalFormat	= format;
//	info.width			= std::min<uint>( maxTextureWidth, (unsigned)GL.MaxTextureSize );
//	info.height			= std::min<uint>( maxTextureHeight, (unsigned)GL.MaxTextureSize );
//	info.index			= 0;
//	info.repeatX		= repeatX;
//	info.repeatY		= repeatY;
//	info.mipmapStyle	= mipmapGeneration;
//	info.filter			= filter;
//	// And prepare the state information
//	state.level_base	= 0;
//	state.level_max		= 0;
//
//	// Check if the texture has a reference
//	const textureEntry_t* pTextureReference = TextureMaster.GetReference( this );
//	// Texture doesn't exist yet
//	if ( pTextureReference == NULL ) 
//	{
//		// Debug output name of loading texture
//		if ( sFilename != sInFilename ) {
//			cout << "New texture: " << sFilename << " (" << sInFilename << ")" << endl;
//		}
//		else {
//			cout << "New texture: " << sInFilename << endl;
//		}
//
//		// Load the image
//		//LoadImageInfo();
//		tPixel tempData;
//		tempData.a = 255;
//		tempData.r = 128;
//		tempData.g = 128;
//		tempData.b = 128;
//
//		// Send the data to OpenGL
//		//HANDLE context = wglGetCurrentContext(); // Only work in current context?
//		glGenTextures( 1, &info.index );
//		// Bind the texture object
//		glBindTexture( GL_TEXTURE_2D, info.index );
//		// Copy the data to the texture object
//		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), 1,1, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, &tempData );
//		// Unbind the data
//		glBindTexture( GL_TEXTURE_2D, 0 );
//
//		// Load the actual data in
//		if ( !RrRenderer::Active->mResourceManager->settings.streamTextures )
//		{
//			// If streaming disabled, then force load
//			RrRenderer::Active->mResourceManager->AddResource( this );
//			RrRenderer::Active->mResourceManager->ForceLoadResource( this );
//		}
//		else
//		{
//			// Disable streaming on certain objects
//			if ( sFilename.find( "textures/hud" ) != string::npos || sFilename.find( "textures/system" ) != string::npos || sFilename.find( "textures/tats" ) != string::npos )
//			{
//				RrRenderer::Active->mResourceManager->AddResource( this );
//				RrRenderer::Active->mResourceManager->ForceLoadResource( this );
//			}
//			// Otherwise, stream the texture
//			else
//			{
//				RrRenderer::Active->mResourceManager->FinishAddResource( this );
//				RrRenderer::Active->mResourceManager->AddResource( this );
//			}
//		}
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
//
//		// That's all we need, since everything else has been set (or will be set) already.
//
//		// Add a reference to the data
//		TextureMaster.AddReference( this );
//		RrRenderer::Active->mResourceManager->AddResource( this );
//	}
//}
//// === Destructor ===
//RrTexture::~RrTexture ( void )
//{
//	// Check for system overrides here
//	if ( info.index == 0 ||  sFilename == "_hx_SYSTEM_FONTLOAD" || sFilename == "_hx_SYSTEM_RENDERTEXTURE" )
//	{	// System overrides should be skipped:
//		return;
//	}
//
//	TextureMaster.RemoveReference( this );
//	if ( RrRenderer::Active )
//		RrRenderer::Active->mResourceManager->RemoveResource( this );
//}
//
//// === Bind and Unbind ===
//void RrTexture::Bind ( void )
//{
//	if ( info.type == Texture2D ) {
//		glDisable( GL_TEXTURE_3D );
//		glDisable( GL_TEXTURE_CUBE_MAP );
//		glEnable( GL_TEXTURE_2D );
//		glBindTexture( GL_TEXTURE_2D, info.index );
//	}
//	else if ( info.type == Texture3D ) {
//		glDisable( GL_TEXTURE_CUBE_MAP );
//		glEnable( GL_TEXTURE_3D );
//		glBindTexture( GL_TEXTURE_3D, info.index );
//	}
//	else if ( info.type == TextureCube ) {
//		glEnable( GL_TEXTURE_CUBE_MAP );
//		//cout << info.index << endl;
//		glBindTexture( GL_TEXTURE_CUBE_MAP, info.index );
//	}
//}
//void RrTexture::Unbind ( void )
//{
//	if ( info.type == Texture2D ) {
//		//glDisable( GL_TEXTURE_2D );
//		glBindTexture( GL_TEXTURE_2D, 0 );
//	}
//	else if ( info.type == Texture3D ) {
//		//glDisable( GL_TEXTURE_3D );
//		glBindTexture( GL_TEXTURE_3D, 0 );
//	}
//	else if ( info.type == TextureCube ) {
//		//glDisable( GL_TEXTURE_CUBE_MAP );
//		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
//	}
//}
//void RrTexture::Unbind ( char a )
//{
//	glBindTexture( GL_TEXTURE_2D, 0 );
//	glBindTexture( GL_TEXTURE_3D, 0 );
//	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
//	glBindTexture( GL_TEXTURE_BUFFER, 0 );
//}

//=========================================//
// === Reloading ===
void RrTexture::Reload ( void )
{
	/*GL_ACCESS;

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
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
	// Copy the data to the texture object
	glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
	// Generate the mipmaps
	GenerateMipmap( info.mipmapStyle );
	// Unbind the data
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Delete the texture data
	delete [] pData;
	pData = NULL;*/
	if (!procedural)
	{
		StreamingReset();
		// Add self back to the reload list of the resource manager:
		auto resm = core::ArResourceManager::Active();
		resm->Remove(this);
		resm->Add(this); // TODO: make this not an awful fucking hack
	}
}
//
////=========================================//
//// === Manual Upload ===
//void RrTexture::Upload (
//	pixel_t* data,
//	uint width,
//	uint height,
//	eWrappingType	repeatX,
//	eWrappingType	repeatY,
//	eMipmapGenerationStyle	mipmapGeneration,
//	eSamplingFilter	filter
//	)
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//
//	// Null out data
//	pData = NULL;
//	// Set the information structure to prepare for reading in
//	info.type			= Texture2D;
//	info.internalFormat	= RGBA8;
//	info.width			= width;
//	info.height			= height;
//	info.repeatX		= repeatX;
//	info.repeatY		= repeatY;
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
//	glBindTexture( GL_TEXTURE_2D, info.index );
//	// Set the pack alignment
//	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
//	// Change the texture repeat
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(info.repeatX) );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(info.repeatY) );
//	// Change the filtering
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
//	// Copy the data to the texture object
//	glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(info.internalFormat), width,height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data );
//	// Generate the mipmaps
//	GenerateMipmap( info.mipmapStyle );
//	// Unbind the data
//	glBindTexture( GL_TEXTURE_2D, 0 );
//}
//
////=========================================//
//// === Mipmap Generation ===
//void RrTexture::GenerateMipmap ( eMipmapGenerationStyle generationStyle )
//{
//	GL_ACCESS;
//
//	switch ( generationStyle ) {
//	case MipmapNone:
//		// Nothing to do.
//		break;
//	case MipmapNormal:
//		// Change filtering
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//		// Generate the mipmaps
//		glGenerateMipmap( GL_TEXTURE_2D );
//		break;
//	case MipmapNearest:
//		// Change filtering
//		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
//		// Change mipmap number
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
//		// Upload mipmaps
//		// Decrease data resolution
//		for ( uint y = 0; y < info.height/2; ++y ) {
//			for ( uint x = 0; x < info.width/2; ++x ) {
//				pData[x+(y*(info.width/2))] = pData[(x+(y*(info.width)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 1, GL.Enum(info.internalFormat), info.width/2, info.height/2, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/4; ++y ) {
//			for ( uint x = 0; x < info.width/4; ++x ) {
//				pData[x+(y*(info.width/4))] = pData[(x+(y*(info.width/2)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 2, GL.Enum(info.internalFormat), info.width/4, info.height/4, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/8; ++y ) {
//			for ( uint x = 0; x < info.width/8; ++x ) {
//				pData[x+(y*(info.width/8))] = pData[(x+(y*(info.width/4)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 3, GL.Enum(info.internalFormat), info.width/8, info.height/8, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/16; ++y ) {
//			for ( uint x = 0; x < info.width/16; ++x ) {
//				pData[x+(y*(info.width/16))] = pData[(x+(y*(info.width/8)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 4, GL.Enum(info.internalFormat), info.width/16, info.height/16, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		for ( uint y = 0; y < info.height/32; ++y ) {
//			for ( uint x = 0; x < info.width/32; ++x ) {
//				pData[x+(y*(info.width/32))] = pData[(x+(y*(info.width/16)))*2];
//			}
//		}
//		glTexImage2D( GL_TEXTURE_2D, 5, GL.Enum(info.internalFormat), info.width/32, info.height/32, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		break;
//	}
//}
//
////=========================================//
//// === Loader ===
//void RrTexture::LoadImageInfo ( void )
//{
//	Textures::timgInfo t_info;
//	pData = Textures::LoadRawImageData( sFilename, t_info );
//	this->info.width			= t_info.width;
//	this->info.height			= t_info.height;
//	this->info.internalFormat	= (eColorFormat)t_info.internalFormat;
//}
//
//
////=========================================//
//// === Setters ===
//void RrTexture::SetFilter ( eSamplingFilter filter )
//{
//	GL_ACCESS;
//	// Update value
//	info.filter = filter;
//	// Bind the texture object
//	glBindTexture( GL_TEXTURE_2D, info.index );
//	// Change the filtering
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum(info.filter) );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum(info.filter) );
//	// Unbind the data
//	glBindTexture( GL_TEXTURE_2D, 0 );
//}
