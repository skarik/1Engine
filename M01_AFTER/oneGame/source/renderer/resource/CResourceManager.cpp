
#include "CResourceManager.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/material/glMaterial.h"
#include "renderer/material/glShader.h"

#include "renderer/texture/TextureLoader.h"
#include "core/math/Math.h"
#include "renderer/texture/CTextureMaster.h"

#include "renderer/system/glMainSystem.h"

#include "zlib/zlib.h"

#include <thread>

static glHandle m_pbo1;
static glHandle m_pbo2;
static glHandle m_pboForce;

static Textures::tbpdHeader m_textureHeader;
static uchar* m_textureBuffer;

CResourceManager::CResourceManager ( void )
	: m_renderStateOwned(false)
{
	// Create the buffers used for streaming
	glGenBuffers( 1, &m_pbo1 );
	glGenBuffers( 1, &m_pbo2 );
	glGenBuffers( 1, &m_pboForce );

	// Upload empty buffer to prevent stall later
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo1 );
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 4096*4096*4, NULL, GL_STATIC_DRAW );
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo2 );
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 4096*4096*4, NULL, GL_STATIC_DRAW );
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboForce );
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 4096*4096*4, NULL, GL_STATIC_DRAW );
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0 );

	// Create buffer for texture data
	m_textureBuffer = new uchar [4096*4096*4];
}

CResourceManager::~CResourceManager ( void )
{
	// Free all resources
	if ( mStreamThread.joinable() ) {
		mStreamThread.join();
	}

	// Delete the streaming buffers now that everything is stopped
	glDeleteBuffers( 1, &m_pbo1 );
	glDeleteBuffers( 1, &m_pbo2 );
	glDeleteBuffers( 1, &m_pboForce );

	delete [] m_textureBuffer; // delete CPU texture buffer
}


void CResourceManager::FileUpdate ( void )
{
#ifdef _ENGINE_DEBUG
	// Debug mode!
#endif
}

void CResourceManager::RenderUpdate ( void )
{
	/*static GLuint m_pboBuffer = 0;
	if ( m_pboBuffer == 0 )
	{
		glGenBuffers( 1, &m_pboBuffer );
	}*/
	GL_ACCESS;

	// Stream in textures
	for ( auto resource = mResourceList.begin(); resource != mResourceList.end(); resource++ )
	{
		// If resource is streaming
		if ( resource->m_needStream && resource->m_resourceType == resourceInfo_t::RESOURCE_TEXTURE )
		{
			// Stream the resource
			CTexture* streamTarget = (CTexture*)resource->m_resource;

			// Begin a thread
			switch ( resource->m_streamState )
			{
			case S_NEED_STREAM:
				if ( mStreamThread.joinable() ) {
					mStreamThread.join();
				}
				mStreamFlag = false; // Set stream flag as not set
				//mStreamThread = std::thread( &CResourceManager::StreamUpdate_Texture, this, streamTarget ); 
				mStreamThread = std::thread( &CResourceManager::StreamUpdate_Texture, this, streamTarget->sFilename.c_str() );
				resource->m_streamState = S_STREAMING;
				return;
			case S_STREAMING:
				// Check if done streaming
				if ( mStreamFlag ) {
					resource->m_streamState = S_NEED_GPU_UPLOAD;
				}
				return;
			case S_NEED_GPU_UPLOAD:
				{
					// Upload the data to the GPU
					uint old_index = streamTarget->info.index;
					//GL.FreeTexture( streamTarget->info.index );
					glDeleteTextures( 1, &streamTarget->info.index );
					glGenTextures( 1, &streamTarget->info.index );
					//streamTarget->info.index = GL.GetNewTexture();
					TextureMaster.UpdateTextureIndex( old_index, streamTarget->info.index );
					glBindTexture( GL_TEXTURE_2D, streamTarget->info.index );
					{
						// Set the pack alignment
						glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
						// Change the texture repeat
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum(streamTarget->info.repeatX) );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum(streamTarget->info.repeatY) );
						// Change the filtering
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
						// Change the lod bias
						glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f );
					}

					Textures::tbpdHeader bpdHeader = m_textureHeader;

					uint32_t t_level;
					uint32_t t_width, t_height;
					uint32_t t_memoryOffset = 0;
					//for ( uint i = 0; i < bpdHeader.levels; i += 1 )
					for ( int i = bpdHeader.levels-1; i >= 0; i -= 1 )
					{
						// Create the level size data
						t_level		= (bpdHeader.levels-1)-i;
						t_width		= bpdHeader.width  / Math.exp2( t_level );
						t_height	= bpdHeader.height / Math.exp2( t_level );

						// Clear the current level's texture (mark storage as proper size)
						glTexImage2D( GL_TEXTURE_2D, t_level, GL.Enum(streamTarget->info.internalFormat), t_width,t_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL );

						// Bind buffer and load data into buffer
						glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_pbo1 );
						glBufferData( GL_PIXEL_UNPACK_BUFFER, t_width*t_height*4, NULL, GL_STATIC_DRAW ); // Upload empty buffer to prevent stall later

						char* m_bufferLocation = (char*)glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, t_width*t_height*4, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
						if ( m_bufferLocation ) {
							// Copy the data into the buffer
							memcpy( m_bufferLocation, m_textureBuffer+t_memoryOffset, t_width*t_height*4 );
							// Unmap the buffer
							glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
						}
						t_memoryOffset += t_width*t_height*4; // Increment the offset into the data buffer
					
						// Use that buffer to upload to texture
						glTexSubImage2D( GL_TEXTURE_2D, t_level, 0,0, t_width,t_height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0 );

						// Clear bound buffer
						glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

						// Swap PBO's to give time to GPU to catch up
						std::swap( m_pbo1, m_pbo2 );
					}
					streamTarget->state.level_base = 0;
					streamTarget->state.level_max = bpdHeader.levels-1;
					//streamTarget->state.level_base = 0;
					//streamTarget->state.level_max = 0;

					if ( bpdHeader.levels > 1 ) {
						// Enable mipmapping
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
					}
					else {
						// Disable mipmapping
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					}

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, streamTarget->state.level_base );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, streamTarget->state.level_max );

					glBindTexture( GL_TEXTURE_2D, 0 );
				}
				// Mark resource as done loading
				resource->m_streamState = S_DONE;
				return;
			case S_DONE:
				resource->m_needStream = false;
				break;
			}
			
			/*{
				// Get the BPD filename
				string t_bpdFilename = streamTarget->sFilename;
				t_bpdFilename = t_bpdFilename.substr( 0, t_bpdFilename.find_last_of( "." ) ) + ".bpd";

				//glBindTexture( GL_TEXTURE_2D, streamTarget->info.index );
				uint old_index = streamTarget->info.index;
				GL.FreeTexture( streamTarget->info.index );
				streamTarget->info.index = GL.GetNewTexture();
				TextureMaster.UpdateTextureIndex( old_index, streamTarget->info.index );
				glBindTexture( GL_TEXTURE_2D, streamTarget->info.index );
				{
					// Set the pack alignment
					glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
					// Change the texture repeat
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, streamTarget->info.repeatX );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, streamTarget->info.repeatY );
					// Change the filtering
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					// Change the lod bias
					glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f );
				}

				// Open the BPD
				FILE* t_bpdFile = fopen( t_bpdFilename.c_str(), "rb" );
				if ( t_bpdFile )
				{
					// Read in the header
					Textures::tbpdHeader bpdHeader;
					fread( &bpdHeader, sizeof(Textures::tbpdHeader), 1, t_bpdFile );

					// Loop through the levels, smallest to largest
					uint32_t t_level;
					uint32_t t_width, t_height;
					//for ( uint i = 0; i < bpdHeader.levels; i += 1 )
					for ( int i = bpdHeader.levels-1; i >= 0; i -= 1 )
					{
						// Create the level size data
						t_level		= (bpdHeader.levels-1)-i;
						t_width		= bpdHeader.width  / Math.exp2( t_level );
						t_height	= bpdHeader.height / Math.exp2( t_level );

						// Seek to the target level
						fseek( t_bpdFile, sizeof(Textures::tbpdHeader)+(sizeof(Textures::tPixel)*16*16)
							+( sizeof(Textures::tbpdLevel) * (i) ), SEEK_SET );
						// Read in that last level data
						Textures::tbpdLevel bpdLevel;
						fread( &bpdLevel, sizeof(Textures::tbpdLevel), 1, t_bpdFile );
						if ( strcmp( bpdLevel.head, "TEX" ) != 0 ) {
							throw std::exception();
						}

						// Clear the current level's texture (mark storage as proper size)
						glTexImage2D( GL_TEXTURE_2D, t_level, streamTarget->info.internalFormat, t_width,t_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL );

						// Bind buffer and load data into buffer
						glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_pbo1 );
						glBufferData( GL_PIXEL_UNPACK_BUFFER, t_width*t_height*4, NULL, GL_STATIC_DRAW ); // Upload empty buffer to prevent stall later

						// Map the buffer to CPU memory
						//char* m_bufferLocation = (char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER,  GL_WRITE_ONLY );
						char* m_bufferLocation = (char*)glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, t_width*t_height*4, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
						if ( m_bufferLocation )
						{
							// Read in the data straight into the buffer
							fseek( t_bpdFile, bpdLevel.offset, SEEK_SET );
							//fread( m_bufferLocation, t_width*t_height*4, 1, t_bpdFile );
							{
								uint32_t t_originalSize;
								fread( &t_originalSize, sizeof(uint32_t),1, t_bpdFile );
								uchar* t_sideBuffer = new uchar [t_originalSize];
								//memcpy( n_side_buffer, n_buffer, n_original_size );
								fread( t_sideBuffer, t_originalSize, 1, t_bpdFile );
								// Decompress the data
								unsigned long t_max_size = bpdHeader.width*bpdHeader.height*4;
								int z_result = uncompress( (uchar*)m_bufferLocation, &t_max_size, (uchar*)t_sideBuffer, t_originalSize );
								switch( z_result )
								{
								case Z_OK:
									break;
								case Z_MEM_ERROR:
									printf("CResourceManager::ForceLoadResource >> out of memory\n");
									throw Engine::OutOfMemoryException();
									break;
								case Z_BUF_ERROR:
									printf("CResourceManager::ForceLoadResource >> output buffer wasn't large enough!\n");
									throw std::out_of_range("Out of space");
									break;
								case Z_DATA_ERROR:
									printf("CResourceManager::ForceLoadResource >> corrupted data!\n");
									throw Engine::CorruptedDataException();
									break;
								}
								// Delete the side buffer
								delete [] t_sideBuffer;
							}
							// Unmap the buffer
							glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
						}

						// Use that buffer to upload to texture
						glTexSubImage2D( GL_TEXTURE_2D, t_level, 0,0, t_width,t_height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0 );

						// Clear bound buffer
						glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

						// Swap PBO's to give time to GPU to catch up
						std::swap( m_pbo1, m_pbo2 );
					}
					// Close BPD file
					fclose( t_bpdFile );

					streamTarget->state.level_base = 0;
					streamTarget->state.level_max = bpdHeader.levels-1;
					//streamTarget->state.level_base = 0;
					//streamTarget->state.level_max = 0;

					if ( bpdHeader.levels > 1 ) {
						// Enable mipmapping
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
					}
					else {
						// Disable mipmapping
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					}
				}

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, streamTarget->state.level_base );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, streamTarget->state.level_max );


				glBindTexture( GL_TEXTURE_2D, 0 );

				resource->m_streamState = 1;
				resource->m_needStream = false;

				break;
			}*/

			// Begin a thread
			/*if ( resource->m_streamState == 0 )
			{
				mStreamFlag = false;
				if ( mStreamThread.joinable() ) {
					mStreamThread.join();
				}
				mStreamThread = std::thread( &CResourceManager::StreamUpdate_Texture, this, streamTarget ); 
				resource->m_streamState = 1;
				break;
			}
			// Check if HDD IO is done
			if ( resource->m_streamState == 1 )
			{
				// The following forces an immediate "streamed" load
				if ( mStreamThread.joinable() ) {
					mStreamThread.join();
				}
				if ( mStreamFlag == true )
				{
					if ( mStreamThread.joinable() ) {
						mStreamThread.join();
					}
					//glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_pboBuffer );
					//glBufferData( GL_PIXEL_UNPACK_BUFFER, sizeof(tPixel)*streamTarget->info.width*streamTarget->info.height, streamTarget->pData, GL_STATIC_DRAW );
			
					glBindTexture( GL_TEXTURE_2D, streamTarget->info.index );
					//glTexImage2D( GL_TEXTURE_2D, 0, streamTarget->info.internalFormat, streamTarget->info.width, streamTarget->info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0 );
					glTexImage2D( GL_TEXTURE_2D, 0, streamTarget->info.internalFormat, streamTarget->info.width, streamTarget->info.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, streamTarget->pData );
					//glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

					//streamTarget->GenerateMipmap( streamTarget->info.mipmapStyle );
					glBindTexture( GL_TEXTURE_2D, 0 );

					delete [] streamTarget->pData;
					streamTarget->pData = NULL;

					resource->m_needStream = false;
					resource->m_streamState = 2;
					break;
				}
			}*/
			// End thread check
		}
		// End stream check
	}
}

void CResourceManager::StreamUpdate_Texture ( const char* n_targetFile )
{
	//n_target->LoadImageInfo();
	//mStreamFlag = true;

	// Get the BPD filename
	string t_bpdFilename = n_targetFile;
	t_bpdFilename = t_bpdFilename.substr( 0, t_bpdFilename.find_last_of( "." ) ) + ".bpd";

	// Open the BPD
	FILE* t_bpdFile = fopen( t_bpdFilename.c_str(), "rb" );
	if ( t_bpdFile )
	{
		// Read in the header
		Textures::tbpdHeader bpdHeader;
		fread( &bpdHeader, sizeof(Textures::tbpdHeader), 1, t_bpdFile );
		// Save the header
		m_textureHeader = bpdHeader;

		// Loop through the levels, smallest to largest
		uint32_t t_level;
		uint32_t t_width, t_height;
		uint32_t t_memoryOffset = 0;
		//for ( uint i = 0; i < bpdHeader.levels; i += 1 )
		for ( int i = bpdHeader.levels-1; i >= 0; i -= 1 )
		{
			// Create the level size data
			t_level		= (bpdHeader.levels-1)-i;
			t_width		= bpdHeader.width  / Math.exp2( t_level );
			t_height	= bpdHeader.height / Math.exp2( t_level );

			// Seek to the target level
			fseek( t_bpdFile, sizeof(Textures::tbpdHeader)+(sizeof(tPixel)*16*16)
				+( sizeof(Textures::tbpdLevel) * (i) ), SEEK_SET );
			// Read in that last level data
			Textures::tbpdLevel bpdLevel;
			fread( &bpdLevel, sizeof(Textures::tbpdLevel), 1, t_bpdFile );
			if ( strcmp( bpdLevel.head, "TEX" ) != 0 ) {
				throw std::exception();
			}

			// Read in the data straight into the buffer
			fseek( t_bpdFile, bpdLevel.offset, SEEK_SET );
			//fread( m_bufferLocation, t_width*t_height*4, 1, t_bpdFile );
			{
				uint32_t t_originalSize;
				// Read in the compressed size
				fread( &t_originalSize, sizeof(uint32_t),1, t_bpdFile );
				// Create buffer to read in data into
				uchar* t_sideBuffer = new uchar [t_originalSize];
				// Read in the compressed data
				fread( t_sideBuffer, t_originalSize, 1, t_bpdFile );
				// Decompress the data
				unsigned long t_max_size = bpdHeader.width*bpdHeader.height*4;
				int z_result = uncompress( (uchar*)m_textureBuffer+t_memoryOffset, &t_max_size, (uchar*)t_sideBuffer, t_originalSize );
				switch( z_result )
				{
				case Z_OK:
					break;
				case Z_MEM_ERROR:
					printf("CResourceManager::StreamUpdate_Texture >> out of memory\n");
					throw Core::OutOfMemoryException();
					break;
				case Z_BUF_ERROR:
					printf("CResourceManager::StreamUpdate_Texture >> output buffer wasn't large enough!\n");
					throw std::out_of_range("Out of space");
					break;
				case Z_DATA_ERROR:
					printf("CResourceManager::StreamUpdate_Texture >> corrupted data!\n");
					throw Core::CorruptedDataException();
					break;
				}
				// Delete the side buffer
				delete [] t_sideBuffer;
			}
			// Increment the memory offset
			t_memoryOffset += t_width*t_height*4;

		}
	}

	// Mark stream operation as done
	mStreamFlag = true;
}

void CResourceManager::AddResource ( CTexture* n_texture )
{
	for ( auto resource = mResourceList.begin(); resource != mResourceList.end(); resource++ )
	{
		if ( resource->m_resource == n_texture )
		{
			return;
		}
	}
	resourceInfo_t newTexture;
	newTexture.m_resource = n_texture;
	newTexture.m_resourceType = resourceInfo_t::RESOURCE_TEXTURE;
	newTexture.m_needReload = false;
	newTexture.m_needStream = true;
	newTexture.m_streamState = S_NEED_STREAM;
	mResourceList.push_back( newTexture );
}

void CResourceManager::RemoveResource ( CTexture* n_texture )
{
	for ( auto resource = mResourceList.begin(); resource != mResourceList.end(); resource++ )
	{
		if ( resource->m_resource == n_texture )
		{
			if ( resource->m_streamState == 1 )
			{
				//mStreamThread.join();
				if ( n_texture->pData ) {
					delete [] n_texture->pData;
				}
				n_texture->pData = NULL;
			}
			mResourceList.erase( resource );
			return;
		}
	}
}

// Force load resources
void CResourceManager::ForceLoadResource ( CTexture* n_texture )
{
	GL_ACCESS;

	for ( auto resource = mResourceList.begin(); resource != mResourceList.end(); resource++ )
	{
		if ( resource->m_resource == n_texture )
		{
			/*if ( resource->m_streamState == 1 )
			{
				mStreamThread.join();
				if ( n_texture->pData ) {
					delete [] n_texture->pData;
				}
				n_texture->pData = NULL;
			}
			mResourceList.erase( resource );
			return;*/
			//RenderUpdate
			/*resourceInfo_t oldData;
			oldData = *resource;
			mResourceList.erase( resource );*/
			//mResourceList.insert( mResourceList.begin(), oldData );
			//RenderUpdate();

			// Mark stream as done
			resource->m_needStream = false;
			resource->m_needReload = false;
			resource->m_streamState = S_DONE;

			// Stream the resource
			CTexture* streamTarget = (CTexture*)resource->m_resource;

			// Get the BPD filename
			string t_bpdFilename = n_texture->sFilename;
			t_bpdFilename = t_bpdFilename.substr( 0, t_bpdFilename.find_last_of( "." ) ) + ".bpd";
			// Convert it to BPD
			try {
				Textures::ConvertFile( n_texture->sFilename, t_bpdFilename );
			}
			catch ( std::exception& e ) {
				printf( "Exception: %s\n", e.what() );
			}
			// Open the BPD
			FILE* t_bpdFile = fopen( t_bpdFilename.c_str(), "rb" );
			if ( t_bpdFile )
			{
				// Read in the header
				Textures::tbpdHeader bpdHeader;
				fread( &bpdHeader, sizeof(Textures::tbpdHeader), 1, t_bpdFile );
				// Skip the lowres data
				fseek( t_bpdFile, sizeof(tPixel)*16*16, SEEK_CUR );
				// Skip all the mipmap data until the last one (that will be the highest resolution one)
				fseek( t_bpdFile, sizeof(Textures::tbpdLevel)*(bpdHeader.levels-1), SEEK_CUR );
				// Read in that last level data
				Textures::tbpdLevel bpdLevel;
				fread( &bpdLevel, sizeof(Textures::tbpdLevel), 1, t_bpdFile );

				// Refresh the resource (delete, remake)
				uint old_index = streamTarget->info.index;
				//GL.FreeTexture( streamTarget->info.index );
				//streamTarget->info.index = GL.GetNewTexture();
				//TextureMaster.UpdateTextureIndex( old_index, streamTarget->info.index );
				// Bind the texture we are to edit
				glBindTexture( GL_TEXTURE_2D, streamTarget->info.index );
				{
				/*	// Set the pack alignment
					glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
					// Change the texture repeat
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, streamTarget->info.repeatX );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, streamTarget->info.repeatY );
					// Change the filtering
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					// Change the lod bias
					glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f );*/
				}

				// Tell driver the size of the function
				glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(streamTarget->info.internalFormat), bpdHeader.width, bpdHeader.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL );
				GL.CheckError();

				// Bind buffer and upload data into it
				glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_pboForce );
				glBufferData( GL_PIXEL_UNPACK_BUFFER, bpdHeader.width*bpdHeader.height*8, NULL, GL_STATIC_DRAW ); // Upload empty buffer to prevent stall later

				// Map the buffer to CPU memory
				//char* m_bufferLocation = (char*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER,  GL_WRITE_ONLY );
				char* m_bufferLocation = (char*)glMapBufferRange( GL_PIXEL_UNPACK_BUFFER, 0, bpdHeader.width*bpdHeader.height*8, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT|GL_MAP_UNSYNCHRONIZED_BIT );
				if ( m_bufferLocation )
				{
					// Read in the data straight into the buffer
					fseek( t_bpdFile, bpdLevel.offset, SEEK_SET );
					//fread( m_bufferLocation, bpdHeader.width*bpdHeader.height*4, 1, t_bpdFile ); 
					{
						uint32_t t_originalSize;
						fread( &t_originalSize, sizeof(uint32_t),1, t_bpdFile );
						uchar* t_sideBuffer = new uchar [t_originalSize];
						//memcpy( n_side_buffer, n_buffer, n_original_size );
						fread( t_sideBuffer, t_originalSize, 1, t_bpdFile );
						// Decompress the data
						unsigned long t_max_size = bpdHeader.width*bpdHeader.height*4;
						int z_result = uncompress( (uchar*)m_bufferLocation, &t_max_size, (uchar*)t_sideBuffer, t_originalSize );
						switch( z_result )
						{
						case Z_OK:
							break;
						case Z_MEM_ERROR:
							printf("CResourceManager::ForceLoadResource >> out of memory\n");
							//throw Engine::OutOfMemoryException();
							break;
						case Z_BUF_ERROR:
							printf("CResourceManager::ForceLoadResource >> output buffer wasn't large enough!\n");
							//throw std::out_of_range("Out of space");
							break;
						case Z_DATA_ERROR:
							printf("CResourceManager::ForceLoadResource >> corrupted data!\n");
							//throw Engine::CorruptedDataException();
							break;
						}
						// Delete the side buffer
						delete [] t_sideBuffer;
					}
					// Unmap the buffer
					glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
				}

				// Give the texture a zero offset
				//glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_pboForce );
				//glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, bpdHeader.width,bpdHeader.height, n_texture->info.internalFormat, GL_UNSIGNED_INT_8_8_8_8_REV, NULL );
				//glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, bpdHeader.width,bpdHeader.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0 );
				glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(streamTarget->info.internalFormat), bpdHeader.width, bpdHeader.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL );
				GL.CheckError();

				// Unbind buffer
				glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0 );

				// Close the file
				fclose( t_bpdFile );
			}

			n_texture->state.level_base = 0;
			n_texture->state.level_max = 0;

			// Disable mipmapping
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );

			// Unbind texture
			glBindTexture( GL_TEXTURE_2D, 0 );

			GL_ACCESS GL.CheckError();
			return;
		}
	}
}


// Finish load resource
void CResourceManager::FinishAddResource ( CTexture* n_texture )
{
	GL_ACCESS
	/*for ( auto resource = mResourceList.begin(); resource != mResourceList.end(); resource++ )
	{
		if ( resource->m_resource == n_texture )
		{

		}
	}*/
	// First convert the file to a BPD
	string t_bpdFilename = n_texture->sFilename;
	t_bpdFilename = t_bpdFilename.substr( 0, t_bpdFilename.find_last_of( "." ) ) + ".bpd";

	// Convert it to BPD
	try {
		Textures::ConvertFile( n_texture->sFilename, t_bpdFilename );
	}
	catch ( std::exception& e ) {
		printf( "Exception: %s\n", e.what() );
	}

	// Now, read in some LQ pixels to start us off
	tPixel* lowQuality = new tPixel [16*16];
	FILE* t_bpdFile = fopen( t_bpdFilename.c_str(), "rb" );
	if ( t_bpdFile )
	{
		Textures::tbpdHeader bpdHeader;
		fread( &bpdHeader, sizeof(Textures::tbpdHeader), 1, t_bpdFile );
		fread( lowQuality, sizeof(tPixel)*16*16, 1, t_bpdFile );
		fclose( t_bpdFile );

		n_texture->info.levels = bpdHeader.levels;

		glBindTexture( GL_TEXTURE_2D, n_texture->info.index );

		// Change filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		// Change mipmap number
		//n_texture->state.level_base = bpdHeader.levels;
		n_texture->state.level_base = 0;
		n_texture->state.level_max = 0;
		//n_texture->state.level_max = bpdHeader.levels;
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, n_texture->state.level_base );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, n_texture->state.level_max );
		
		// Upload the minimum guy as a mipmap
		/*for ( uint i = 0; i <= bpdHeader.levels; i++ )
		{
			glTexImage2D( GL_TEXTURE_2D, i, n_texture->info.internalFormat, 16, 16, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, lowQuality );
		}*/
		glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum(n_texture->info.internalFormat), 16, 16, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, lowQuality );

		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	delete [] lowQuality;
}