#include "RrTextureCube.h"

#include "core/system/io/FileUtils.h"
#include "core/utils/string.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/resources/ResourceManager.h"

#include "core-ext/threads/Jobs.h" // TODO: hacky speed increase

#include "gpuw/Buffers.h"

RrTextureCube::RrTextureCube (
	const char* s_resourceId,
	const char* s_resourcePath
	)
	: RrTexture(s_resourceId, s_resourcePath)
{
	// All handled by RrTexture.
}

RrTextureCube::~RrTextureCube ( void )
{
	// All handled by RrTexture.
}

#include "core-ext/system/io/assets/TextureIO.h"

RrTextureCube* RrTextureCube::Load ( const char* resource_name )
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
		ARCORE_ASSERT(existingTexture->ClassType() == core::gfx::tex::kTextureClassCube);
		existingTexture->AddReference(); // TODO: the docs say this doesn't happen. Evaluate and remove this.
		return (RrTextureCube*)existingTexture;
	}

	// We need to create a new texture:
	RrTextureCube* texture = new RrTextureCube(resource_str_id, NULL);

	// Need to load in all the matching resources with the _N pattern added.
	for (int faceIndex = 1; faceIndex <= 6; ++faceIndex)
	{
		std::string resource_name_to_load = std::string(resource_name) + '_' + std::to_string(faceIndex);

		core::BpdLoader loader;
		loader.m_loadImageInfo = true;
		auto loadStatus = loader.LoadBpd(resource_name_to_load.c_str());
		loader.m_loadImageInfo = false;
		ARCORE_ASSERT(loadStatus);

		if (faceIndex == 1)
		{
			texture->info.type = core::gfx::tex::kTextureTypeCube;
			texture->info.internalFormat = core::gfx::tex::kColorFormatRGBA8;
			texture->info.width = loader.info.width;
			texture->info.height = loader.info.height;
			texture->info.depth = 1;
			texture->info.levels = 1;

			// allocate texture now
			texture->m_texture.allocate(texture->info.type, texture->info.internalFormat, texture->info.width, texture->info.height, texture->info.depth, texture->info.levels);
		}
		else
		{
			ARCORE_ASSERT(texture->info.width == loader.info.width);
			ARCORE_ASSERT(texture->info.height == loader.info.height);
		}

		// set up upload buffer
		gpu::Buffer pixelBuffer;
		pixelBuffer.initAsTextureBuffer(NULL, texture->info.type, texture->info.internalFormat, texture->info.width, texture->info.height, 1);

		uint32 target_pitch = 0;
		void* target = pixelBuffer.map(NULL, gpu::kTransferWriteDiscardPrevious, target_pitch);

		// load in image data for the face
		loader.m_buffer_Mipmaps[0] = target;
		loader.m_loadMipmapMask = 0x01;
		loader.LoadBpd();
		loader.m_buffer_Mipmaps[0] = NULL;

		// Select correct face
		int sliceIndex = 0;
		switch ( faceIndex - 1 )
		{
			// North
		case 0: sliceIndex = 2; break;
			// East
		case 1: sliceIndex = 0; break;
			// South
		case 2: sliceIndex = 3; break;
			// West
		case 3: sliceIndex = 1; break;
			// Top
		case 4: sliceIndex = 4; break;
			// Bottom
		case 5: sliceIndex = 5; break;
		}

		// Transform the data based on the slice
		if (sliceIndex == 3 || sliceIndex == 4 || sliceIndex == 5)
		{
			// Flip on X
			const uint32 elementSize = core::gfx::tex::getColorFormatByteSize(texture->info.internalFormat);
			const uint32 elementRowSize = texture->info.width * elementSize;

			for (uint16_t row = 0; row < texture->info.height; ++row)
			{
				core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeEngine, [texture, target, row, elementRowSize, elementSize]() {
					for (uint16_t column = 0; column < texture->info.width / 2; ++column)
					{
						char elementTempBuffer [sizeof(uint32) * 4];
						memcpy(elementTempBuffer, (char*)target + row * elementRowSize + column * elementSize, elementSize);
						memcpy((char*)target + row * elementRowSize + column * elementSize, (char*)target + row * elementRowSize + (texture->info.width - column - 1) * elementSize, elementSize);
						memcpy((char*)target + row * elementRowSize + (texture->info.width - column - 1) * elementSize, elementTempBuffer, elementSize);
					}
				});
			}
			core::jobs::System::Current::WaitForJobs(core::jobs::kJobTypeEngine);
		}
		else if (sliceIndex == 2)
		{
			// Flip on Y
			const uint32 elementSize = core::gfx::tex::getColorFormatByteSize(texture->info.internalFormat);
			const uint32 elementRowSize = texture->info.width * elementSize;

			for (uint16_t row = 0; row < texture->info.height / 2; ++row)
			{
				core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeEngine, [texture, target, row, elementRowSize, elementSize]() {
					char* elementTempBuffer = new char [elementRowSize];
					memcpy(elementTempBuffer, (char*)target + row * elementRowSize, elementRowSize);
					memcpy((char*)target + row * elementRowSize, (char*)target + (texture->info.height - row - 1) * elementRowSize, elementRowSize);
					memcpy((char*)target + (texture->info.height - row - 1) * elementRowSize, elementTempBuffer, elementRowSize);
					delete[] elementTempBuffer;
				});
			}
			core::jobs::System::Current::WaitForJobs(core::jobs::kJobTypeEngine);
		}
		else if (sliceIndex == 0)
		{
			// Flip on +X/+Y
			const uint32 elementSize = core::gfx::tex::getColorFormatByteSize(texture->info.internalFormat);
			const uint32 elementRowSize = texture->info.width * elementSize;

			ARCORE_ASSERT(texture->info.width == texture->info.height);
			for (uint16_t row = 0; row < texture->info.height; ++row)
			{
				core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeEngine, [texture, target, row, elementRowSize, elementSize]() {
					for (uint16_t column = 0; column < row; ++column)
					{
						char elementTempBuffer [sizeof(uint32) * 4];
						memcpy(elementTempBuffer, (char*)target + row * elementRowSize + column * elementSize, elementSize);
						memcpy((char*)target + row * elementRowSize + column * elementSize, (char*)target + column * elementRowSize + row * elementSize, elementSize);
						memcpy((char*)target + column * elementRowSize + row * elementSize, elementTempBuffer, elementSize);
					}
				});
			}
			core::jobs::System::Current::WaitForJobs(core::jobs::kJobTypeEngine);
		}
		else if (sliceIndex == 1)
		{
			// Flip on -X/+Y
			const uint32 elementSize = core::gfx::tex::getColorFormatByteSize(texture->info.internalFormat);
			const uint32 elementRowSize = texture->info.width * elementSize;

			ARCORE_ASSERT(texture->info.width == texture->info.height);
			for (uint16_t row = 0; row < texture->info.height; ++row)
			{
				core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeEngine, [texture, target, row, elementRowSize, elementSize]() {
					for (uint16_t column = 0; column < (texture->info.width - row - 1); ++column)
					{
						char elementTempBuffer [sizeof(uint32) * 4];
						memcpy(elementTempBuffer, (char*)target + row * elementRowSize + column * elementSize, elementSize);
						memcpy((char*)target + row * elementRowSize + column * elementSize, (char*)target + (texture->info.width - column - 1) * elementRowSize + (texture->info.height - row - 1) * elementSize, elementSize);
						memcpy((char*)target + (texture->info.width - column - 1) * elementRowSize + (texture->info.height - row - 1) * elementSize, elementTempBuffer, elementSize);
					}
				});
			}
			core::jobs::System::Current::WaitForJobs(core::jobs::kJobTypeEngine);
		}

		// Loop from the bottom row of the texture, and move the data so the pitches are loaded into the correct spot.
		const uint32 assumed_pitch = texture->info.width * (uint32)core::gfx::tex::getColorFormatByteSize(texture->info.internalFormat);
		if (target_pitch != assumed_pitch)
		{
			for (int16_t row = texture->info.height - 1; row > 0; --row)
			{
				memcpy((char*)target + row * target_pitch, (char*)target + row * assumed_pitch, assumed_pitch);
			}
		}

		// Unmap & upload
		pixelBuffer.unmap(NULL);
		texture->m_texture.upload(NULL, pixelBuffer, 0, sliceIndex);
	}

	// Add it to the resource system:
	resm->Add(texture);

	return texture;
}

//
//// Includes
//#include "core/system/io/FileUtils.h"
//#include "renderer/state/RrRenderer.h"
//#include "RrTextureCube.h"
//#include "RrTextureMaster.h"
//#include "renderer/system/glMainSystem.h"
//#include "core-ext/system/io/Resources.h"
//
//// === Constructor ===
//RrTextureCube::RrTextureCube ( const string &sInIdentifier,
//		const string& sInFilenameXPos, const string& sInFilenameXNeg,
//		const string& sInFilenameYPos, const string& sInFilenameYNeg,
//		const string& sInFilenameZPos, const string& sInFilenameZNeg,
//		eTextureType	textureType,
//		eColorFormat	format,
//		eMipmapGenerationStyle	mipmapGeneration
//		) : RrTexture( "_hx_SYSTEM_SKIP" )
//{
//	GL_ACCESS; // Require access to the GL system.
//
//	// Ensure proper format passed in
//	if ( textureType != TextureCube ) {
//		throw std::exception();
//	}
//
//	sFilename = sInIdentifier;
//	// Check for system overrides here
//	if (( sFilename == "_hx_SYSTEM_FONTLOAD" )||( sFilename == "_hx_SYSTEM_RENDERTEXTURE" )||( sFilename == "_hx_SYSTEM_SKIP" )) {
//		return;
//	}
//	// Look for null texture request (just a fast white sampler)
//	if ( sFilename == "null" || sFilename == "Null" || sFilename == "NULL" ) {
//		sFilename = "textures/null.jpg";
//	}
//	// Standardize the filename
//	sFilename = IO::FilenameStandardize( sFilename );
//
//	// Set the information structure to prepare for reading in
//	info.type			= textureType;
//	info.internalFormat	= format;
//	info.width			= (unsigned)GL.MaxTextureSize;
//	info.height			= (unsigned)GL.MaxTextureSize;
//	info.depth			= (unsigned)GL.MaxTextureSize;
//	info.index			= 0;
//	info.mipmapStyle	= mipmapGeneration;
//	
//	// Check if the texture has a reference
//	const textureEntry_t* pTextureReference = TextureMaster.GetReference( this );
//	// Texture doesn't exist yet
//	if ( pTextureReference == NULL ) 
//	{
//		// Send the data to OpenGL
//		//info.index = GL.GetNewTexture();
//		glGenTextures( 1, &info.index );
//		// Bind the texture object
//		glBindTexture( GL_TEXTURE_CUBE_MAP, info.index );
//		// Set the pack alignment
//		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
//
//		// Copy the data to the texture object
//		sFilename = core::Resources::PathTo( sInFilenameXPos );
//		LoadImageInfo();
//		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		delete [] pData;
//		pData = NULL;
//
//		sFilename = core::Resources::PathTo( sInFilenameXNeg );
//		LoadImageInfo();
//		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		delete [] pData;
//		pData = NULL;
//
//		sFilename = core::Resources::PathTo( sInFilenameYPos );
//		LoadImageInfo();
//		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		delete [] pData;
//		pData = NULL;
//
//		sFilename = core::Resources::PathTo( sInFilenameYNeg );
//		LoadImageInfo();
//		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		delete [] pData;
//		pData = NULL;
//
//		sFilename = core::Resources::PathTo( sInFilenameZPos );
//		LoadImageInfo();
//		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		delete [] pData;
//		pData = NULL;
//
//		sFilename = core::Resources::PathTo( sInFilenameZNeg );
//		LoadImageInfo();
//		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL.Enum(info.internalFormat), info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pData );
//		delete [] pData;
//		pData = NULL;
//
//		// Set blending mode
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//		// Unbind the data
//		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
//		
//		sFilename = sInIdentifier;
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