#include "TextureIO.h"

#include "core/utils/string.h"
#include "core/system/io/CBinaryFile.h"
#include "core/math/Math.h"
#include "core/debug/console.h"
#include "core/exceptions.h"
#include "core/system/io/CMappedBinaryFile.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/Conversion.h"

#include "zlib/zlib.h"

core::BpdWriter::BpdWriter()
	: m_generateMipmaps(false), m_writeAnimation(false), m_convertAndEmbedPalette(false),
	// Inputs:
	rawImageFormat(IMG_FORMAT_RGBA8), rawImage(NULL), mipmaps(), mipmapCount(0),
	palette(NULL), paletteRows(0), paletteDepth(0),
	info(), animationInfo(), frame_times(), datetime(0)
{
	;
}
core::BpdWriter::~BpdWriter()
{
}

bool core::BpdWriter::WriteBpd ( const char* n_newfilename )
{
	if (datetime == 0)
	{
		debug::Console->PrintError( "BpdWriter::WriteBpd : datetime must be set.\n" );
		return false;
	}

	// Check the data
	{
		if (info.width == 0) {
			info.width = 1;
		}
		if (info.height == 0) {
			info.height = 1;
		}
		if (info.depth == 0) {
			info.depth = 1;
		}
	}

	// Open up the file
	CMappedBinaryFile mappedfile = CMappedBinaryFile(n_newfilename);
	if (!mappedfile.GetReady())
	{
		return false;
	}
	m_file = &mappedfile;

	// Begin with writing the header
	if (!writeHeader() ||
		!writeSuperlow() ||
		!writeLevelData() ||
		!writeAnimation() ||
		!writePalette() ||
		!patchHeader() ||
		!patchLevels())
	{
		mappedfile.SyncToDisk();
		return false;
	}

	// Write the extra fun times at the end:
	const char l_idString[] = "END OF THE LINE, ASSHOLE";
	mappedfile.SeekTo(mappedfile.GetSize());
	mappedfile.WriteBuffer(l_idString, sizeof(l_idString));

	// Close the file
	mappedfile.SyncToDisk();

	return true;
}

//	writeHeader() : writes header
bool core::BpdWriter::writeHeader ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	textureFmtHeader l_header = {};
	mappedfile->WriteBuffer(&l_header, sizeof(textureFmtHeader));

	return true;
}
//	patchHeader() : fixes up the data in the header
bool core::BpdWriter::patchHeader( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	textureFmtHeader l_header = {};
	l_header.head[0] = kTextureFormat_Header[0];
	l_header.head[1] = kTextureFormat_Header[1];
	l_header.head[2] = kTextureFormat_Header[2];
	l_header.head[3] = kTextureFormat_Header[3];
	l_header.version[0] = kTextureFormat_VersionMajor;
	l_header.version[1] = kTextureFormat_VersionMinor;
	l_header.datetime	= datetime;

	l_header.flags = 0;
	// Byte 0: Format
	l_header.flags		|= rawImageFormat & 0x000000FF;
	// Byte 1: Transparency & load modes
	l_header.flags		|= (ALPHA_LOAD_MODE_DEFAULT << 8) & 0x0000FF00;
	// Byte 2: Type of texture (animated, paletted?)
	l_header.flags		|= ((m_writeAnimation ? 0x01 : 0) << 16) & 0x00FF0000;
	l_header.flags		|= ((m_offsetPalette  ? 0x02 : 0) << 16) & 0x00FF0000;
	// Byte 3: Type of texture 2 (2D, 3D?)
	l_header.flags		|= (((int)info.type) << 24) & 0xFF000000;

	l_header.width		= info.width;
	l_header.height		= info.height;
	l_header.depth		= info.depth;
	l_header.levels		= info.levels;

	l_header.levelsOffset	= m_offsetLevels;
	l_header.animationOffset= m_offsetAnimation;
	l_header.paletteOffset	= m_offsetPalette;
	l_header.normalsOffset	= 0;
	l_header.surfaceOffset	= 0;
	l_header.illuminOffset	= 0;

	// Write the header:
	mappedfile->SeekTo(0);
	mappedfile->WriteBuffer(&l_header, sizeof(textureFmtHeader));

	return true;
}

//	writeSuperlow() : generates & writes the superlow texture variant
bool core::BpdWriter::writeSuperlow ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;
	
	ARCORE_ASSERT(rawImage != NULL);
	ARCORE_ASSERT((rawImageFormat == IMG_FORMAT_RGBA8)
		|| (rawImageFormat == IMG_FORMAT_RGBA16)
		|| (rawImageFormat == IMG_FORMAT_RGBA16F)
		|| (rawImageFormat == IMG_FORMAT_PALLETTE)); // Only RGB formats are supported so far

	uint32_t formatFlags = 0;
	formatFlags = (0xFF & rawImageFormat);

	// Allocate room for the LQ data
	char* lowQuality = new char [kTextureFormat_SuperlowByteSize];
	memset(lowQuality, 0, kTextureFormat_SuperlowByteSize);

	uint64_t	aggregate_r, aggregate_g, aggregate_b, aggregate_a;
	uint32_t	set_w, set_h;
	uint32_t	pixelIndex;
	// Calculate amount of pixels we have to downscale over
	set_w = std::max<uint>( info.width / kTextureFormat_SuperlowWidth, 1 );
	set_h = std::max<uint>( info.height/ kTextureFormat_SuperlowWidth, 1 );
	// Loop through the image, scaling downward and sampling to get all the correct information.
	for ( uint x = 0; x < kTextureFormat_SuperlowWidth; x += 1 )
	{
		for ( uint y = 0; y < kTextureFormat_SuperlowWidth; y += 1 )
		{
			// Reset aggregates
			aggregate_r = 0; aggregate_g = 0; aggregate_b = 0; aggregate_a = 0;
			for ( uint sx = set_w * x; sx < set_w * (x + 1); sx += 1 )
			{
				for ( uint sy = set_h * y; sy < set_h * (y + 1); sy += 1 )
				{
					pixelIndex = sx + sy * info.width;
					// Set the column
					if ( set_w != 1 ) 
						pixelIndex = sx;
					else 
						pixelIndex = x / std::max(kTextureFormat_SuperlowWidth / info.width, 1);
					// Add the row
					if ( set_h != 1 ) 
						pixelIndex += sy * info.width;
					else
						pixelIndex += (y / std::max(kTextureFormat_SuperlowWidth / info.height, 1)) * info.width;
					// Clamp the position
					pixelIndex = std::min<uint32_t>( pixelIndex, (uint32_t)(info.width * info.height) - 1 );
					// Collect the colors
					if (rawImageFormat == IMG_FORMAT_RGBA8)
					{
						gfx::tex::vecRGBA8* rawImageRgba = static_cast<gfx::tex::vecRGBA8*>(rawImage);
						aggregate_r += rawImageRgba[pixelIndex].r;
						aggregate_g += rawImageRgba[pixelIndex].g;
						aggregate_b += rawImageRgba[pixelIndex].b;
						aggregate_a += rawImageRgba[pixelIndex].a;
					}
					else if (rawImageFormat == IMG_FORMAT_RGB8)
					{
						gfx::tex::vecRGB8* rawImageRgb = static_cast<gfx::tex::vecRGB8*>(rawImage);
						aggregate_r += rawImageRgb[pixelIndex].r;
						aggregate_g += rawImageRgb[pixelIndex].g;
						aggregate_b += rawImageRgb[pixelIndex].b;
						aggregate_a += 255;
					}
					else if (rawImageFormat == IMG_FORMAT_PALLETTE)
					{
						gfx::tex::vecXY8* rawImageXy = static_cast<gfx::tex::vecXY8*>(rawImage);
						if (rawImageXy[pixelIndex].x != 255 && rawImageXy[pixelIndex].y != 255)
						{
							aggregate_r += rawImageXy[pixelIndex].x;
							aggregate_g += rawImageXy[pixelIndex].y;
							aggregate_b += 255;
							aggregate_a += 255;
						}
						else
						{
							float aggregate_ratio = (float)(1 + sx + sy * set_w);
							aggregate_r += (uint64_t)(aggregate_r / aggregate_ratio);
							aggregate_g += (uint64_t)(aggregate_g / aggregate_ratio);
						}
					}
					else
					{
						ARCORE_ERROR("Unsupported format");
					}
				}
			}
			// Average the colors
			const int set_sz = set_w * set_h;
			aggregate_r /= set_sz;
			aggregate_g /= set_sz;
			aggregate_b /= set_sz;
			aggregate_a /= set_sz;

			// Output onto the superlow buffer
			const int outputIndx = x + y * kTextureFormat_SuperlowWidth;
			if (rawImageFormat == IMG_FORMAT_RGBA8)
			{
				gfx::tex::vecRGBA8* lowQualityRgba = static_cast<gfx::tex::vecRGBA8*>((void*)lowQuality);
				lowQualityRgba[outputIndx].r = (uint8_t)std::min<uint64_t>( aggregate_r, 255 );
				lowQualityRgba[outputIndx].g = (uint8_t)std::min<uint64_t>( aggregate_g, 255 );
				lowQualityRgba[outputIndx].b = (uint8_t)std::min<uint64_t>( aggregate_b, 255 );
				lowQualityRgba[outputIndx].a = (uint8_t)std::min<uint64_t>( aggregate_a, 255 );
			}
			else if (rawImageFormat == IMG_FORMAT_RGB8)
			{
				gfx::tex::vecRGB8* lowQualityRgba = static_cast<gfx::tex::vecRGB8*>((void*)lowQuality);
				lowQualityRgba[outputIndx].r = (uint8_t)std::min<uint64_t>( aggregate_r, 255 );
				lowQualityRgba[outputIndx].g = (uint8_t)std::min<uint64_t>( aggregate_g, 255 );
				lowQualityRgba[outputIndx].b = (uint8_t)std::min<uint64_t>( aggregate_b, 255 );
			}
			else if (rawImageFormat == IMG_FORMAT_PALLETTE)
			{
				gfx::tex::vecXY8* lowQualityXy = static_cast<gfx::tex::vecXY8*>((void*)lowQuality);
				if (aggregate_a >= 127)
				{
					lowQualityXy[outputIndx].x = (uint8_t)std::min<uint64_t>( aggregate_r, 255 );
					lowQualityXy[outputIndx].y = (uint8_t)std::min<uint64_t>( aggregate_g, 255 );
				}
				else
				{
					lowQualityXy[outputIndx].x = 255;
					lowQualityXy[outputIndx].y = 255;
				}
			}
			else
			{
				ARCORE_ERROR("Unsupported format");
			}
		}
	}

	// Write out the format flags
	mappedfile->WriteBuffer(&formatFlags, sizeof(uint32_t));
	// Write out the low-quality guys
	mappedfile->WriteBuffer(lowQuality, kTextureFormat_SuperlowByteSize);

	// Remove the LQ data
	delete[] lowQuality;

	return true;
}

bool core::BpdWriter::writeLevelData ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	ARCORE_ASSERT(rawImage != nullptr);

	// Level stopper: defines the smallest mipmap size. Set to 3 to stop at 16x16. Set to 4 for 32x32. -1 for 1x1.
	const int kLevelStopper = 3;
	uint16_t level_count = (!m_generateMipmaps) ? std::max<int>( 1, info.levels ) : (uint16_t) std::max<int>( 1, math::log2( std::min<uint>(info.width, info.height) ) - kLevelStopper ); 
	info.levels = (uint8)level_count;

	// Resize levels
	m_levels.resize(level_count);
	// Write all the level info out as a spacer
	m_offsetLevels = (uint32_t)mappedfile->GetCursor();
	mappedfile->WriteBuffer(m_levels.data(), sizeof(textureFmtLevel) * m_levels.size());

	// Now, allocate the compression and conversion buffers:
	const size_t	pixelByteSize = getTextureFormatByteSize(rawImageFormat);
	const size_t	largestMipByteCount = pixelByteSize * info.width * info.height * info.depth;
	void*			mipmapBuffer = m_generateMipmaps ? new char [largestMipByteCount] : NULL;
	uint32_t		compressBufferLen = compressBound((uint32_t)largestMipByteCount);
	uchar*			compressBuffer = new uchar [compressBufferLen + 1];

	// Write the levels:
	for ( int level = level_count - 1; level >= 0; --level ) 
	{	// Looping from smallest level to the raw:
		uint32_t	t_blocks = math::exp2( level );
		uint32_t	t_width  = std::max<uint32>( 1, info.width  / t_blocks );
		uint32_t	t_height = std::max<uint32>( 1, info.height / t_blocks );
		uint32_t	t_depth  = std::max<uint32>( 1, info.depth  / t_blocks );

		if (m_generateMipmaps)
		{
			uint32_t	aggregate_r, aggregate_g, aggregate_b, aggregate_a;
			uint32_t	pixelIndex;

			// Downsample the image
			for ( uint x = 0; x < t_width; x += 1 )
			{
				for ( uint y = 0; y < t_height; y += 1 )
				{
					// Reset aggregates
					aggregate_r = 0; aggregate_g = 0; aggregate_b = 0; aggregate_a = 0;
					// Collect the colors
					for ( uint sx = 0; sx < t_blocks; sx += 1 )
					{
						for ( uint sy = 0; sy < t_blocks; sy += 1 )
						{
							pixelIndex = (x * t_blocks + sx) + (y * t_blocks + sy) * info.width;
							if (rawImageFormat == IMG_FORMAT_RGBA8)
							{
								gfx::tex::vecRGBA8* rawImageRgba = static_cast<gfx::tex::vecRGBA8*>(rawImage);
								aggregate_r += rawImageRgba[pixelIndex].r;
								aggregate_g += rawImageRgba[pixelIndex].g;
								aggregate_b += rawImageRgba[pixelIndex].b;
								aggregate_a += rawImageRgba[pixelIndex].a;
							}
							else if (rawImageFormat == IMG_FORMAT_RGB8)
							{
								gfx::tex::vecRGB8* rawImageRgb = static_cast<gfx::tex::vecRGB8*>(rawImage);
								aggregate_r += rawImageRgb[pixelIndex].r;
								aggregate_g += rawImageRgb[pixelIndex].g;
								aggregate_b += rawImageRgb[pixelIndex].b;
								aggregate_a += 255;
							}
							else if (rawImageFormat == IMG_FORMAT_PALLETTE)
							{
								gfx::tex::vecXY8* rawImageXy = static_cast<gfx::tex::vecXY8*>(rawImage);
								if (rawImageXy[pixelIndex].x != 255 && rawImageXy[pixelIndex].y != 255)
								{
									aggregate_r += rawImageXy[pixelIndex].x;
									aggregate_g += rawImageXy[pixelIndex].y;
									aggregate_b += 255;
									aggregate_a += 255;
								}
								else
								{
									float aggregate_ratio = (float)(1 + sx + sy * t_blocks);
									aggregate_r += (uint64_t)(aggregate_r / aggregate_ratio);
									aggregate_g += (uint64_t)(aggregate_g / aggregate_ratio);
								}
							}
							else
							{
								ARCORE_ERROR("Unsupported format");
							}
						}
					}

					// Average the colors
					const uint32_t block_sz = t_blocks * t_blocks;
					aggregate_r /= block_sz;
					aggregate_g /= block_sz;
					aggregate_b /= block_sz;
					aggregate_a /= block_sz;

					// Output into the mipmap buffer
					const uint32_t outputIndx = x + y * t_width;
					if (rawImageFormat == IMG_FORMAT_RGBA8)
					{
						gfx::tex::vecRGBA8* mipmapBufferRgba = static_cast<gfx::tex::vecRGBA8*>((void*)mipmapBuffer);
						mipmapBufferRgba[outputIndx].r = (uint8_t)std::min<uint64_t>( aggregate_r, 255 );
						mipmapBufferRgba[outputIndx].g = (uint8_t)std::min<uint64_t>( aggregate_g, 255 );
						mipmapBufferRgba[outputIndx].b = (uint8_t)std::min<uint64_t>( aggregate_b, 255 );
						mipmapBufferRgba[outputIndx].a = (uint8_t)std::min<uint64_t>( aggregate_a, 255 );
					}
					else if (rawImageFormat == IMG_FORMAT_RGB8)
					{
						gfx::tex::vecRGB8* mipmapBufferRgba = static_cast<gfx::tex::vecRGB8*>((void*)mipmapBuffer);
						mipmapBufferRgba[outputIndx].r = (uint8_t)std::min<uint64_t>( aggregate_r, 255 );
						mipmapBufferRgba[outputIndx].g = (uint8_t)std::min<uint64_t>( aggregate_g, 255 );
						mipmapBufferRgba[outputIndx].b = (uint8_t)std::min<uint64_t>( aggregate_b, 255 );
					}
					else if (rawImageFormat == IMG_FORMAT_PALLETTE)
					{
						gfx::tex::vecXY8* mipmapBufferXy = static_cast<gfx::tex::vecXY8*>((void*)mipmapBuffer);
						if (aggregate_a >= 127)
						{
							mipmapBufferXy[outputIndx].x = (uint8_t)std::min<uint64_t>( aggregate_r, 255 );
							mipmapBufferXy[outputIndx].y = (uint8_t)std::min<uint64_t>( aggregate_g, 255 );
						}
						else
						{
							mipmapBufferXy[outputIndx].x = 255;
							mipmapBufferXy[outputIndx].y = 255;
						}
					}
					else
					{
						ARCORE_ERROR("Unsupported format");
					}
				}
			}
		} // End downsampling

		// Compress the data into side buffer
		unsigned long compressedSize = compressBufferLen;
		size_t uncompressedSize = pixelByteSize * t_width * t_height * t_depth;
		int z_result = compress(compressBuffer, &compressedSize, (uchar*)(m_generateMipmaps ? mipmapBuffer : mipmaps[level]), (uint32_t)(uncompressedSize));

		// Check the compress result
		switch( z_result )
		{
		case Z_OK:
			break;
		case Z_MEM_ERROR:
			debug::Console->PrintError("BpdWriter::writeLevelData : zlib : out of memory\n");
			throw std::out_of_range("Out of memory");
			break;
		case Z_BUF_ERROR:
			debug::Console->PrintError("BpdWriter::writeLevelData : zlib : output buffer wasn't large enough\n");
			throw std::out_of_range("Out of space");
			break;
		}

		// Save the current offset and other data
		m_levels[level].offset	= (uint32_t)mappedfile->GetCursor();
		m_levels[level].size	= (uint32_t)compressedSize;
		m_levels[level].level	= level;

		// Write compressed image to the file
		mappedfile->WriteBuffer(compressBuffer, compressedSize);
	}

	// Free temp info:
	delete[] mipmapBuffer;
	delete[] compressBuffer;

	return true;
}
//	patchLevels() : fixes up the data in the level data
bool core::BpdWriter::patchLevels( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	// Patch up the level headers
	for (size_t i = 0; i < m_levels.size(); ++i)
	{
		m_levels[i].head[0] = kTextureFormat_HeadLevel[0];
		m_levels[i].head[1] = kTextureFormat_HeadLevel[1];
		m_levels[i].head[2] = kTextureFormat_HeadLevel[2];
		m_levels[i].head[3] = kTextureFormat_HeadLevel[3];
	}

	// Write all the level info out as a spacer
	mappedfile->SeekTo(m_offsetLevels);
	mappedfile->WriteBuffer(m_levels.data(), sizeof(textureFmtLevel) * m_levels.size());

	return true;
}

//	writeAnimation() : writes animation needed
bool core::BpdWriter::writeAnimation ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	if (!m_writeAnimation)
	{
		m_offsetAnimation = 0;
		return true;
	}

	textureFmtAnimation l_animation = {};
	l_animation.head[0] = kTextureFormat_HeadAnimation[0];
	l_animation.head[1] = kTextureFormat_HeadAnimation[1];
	l_animation.head[2] = kTextureFormat_HeadAnimation[2];
	l_animation.head[3] = kTextureFormat_HeadAnimation[3];

	l_animation.frames = animationInfo.framecount;
	l_animation.xdivs  = animationInfo.xdivs;
	l_animation.ydivs  = animationInfo.ydivs;
	l_animation.framerate = animationInfo.framerate;
	
	// Write animation info now
	m_offsetAnimation = (uint32_t)mappedfile->GetCursor();
	mappedfile->WriteBuffer(&l_animation, sizeof(textureFmtAnimation));

	// Now write the animations
	for (uint i = 0; i < animationInfo.framecount; ++i)
	{
		textureFmtFrame l_frame = {};
		l_frame.index = i;
		l_frame.duration = frame_times ? frame_times[i] : 1;

		mappedfile->WriteBuffer(&l_frame, sizeof(textureFmtFrame));
	}

	return true;
}
//	writePalette() : writes palette needed
bool core::BpdWriter::writePalette ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	if (palette == NULL)
	{
		m_offsetPalette = 0;
		return true;
	}

	textureFmtPalette l_palette = {};
	l_palette.head[0] = kTextureFormat_HeadPalette[0];
	l_palette.head[1] = kTextureFormat_HeadPalette[1];
	l_palette.head[2] = kTextureFormat_HeadPalette[2];
	l_palette.head[3] = kTextureFormat_HeadPalette[3];

	l_palette.depth	= paletteDepth;
	l_palette.rows	= paletteRows;

	// Write palette info now
	m_offsetPalette = (uint32_t)mappedfile->GetCursor();
	mappedfile->WriteBuffer(&l_palette, sizeof(textureFmtPalette));

	// Write the palette itself
	mappedfile->WriteBuffer(palette, sizeof(gfx::arPixel) * paletteRows * paletteDepth);

	return true;
}
