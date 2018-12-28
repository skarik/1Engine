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

core::BpdLoader::BpdLoader()
	: m_loadOnlySuperlow(false), m_loadImageInfo(false), m_loadMipmapMask(0), m_loadPalette(false), m_loadAnimation(false),
	// Buffer Outputs:
	m_buffer_Superlow(NULL), m_buffer_Mipmaps(),
	// Outputs:
	mipmapCount(0), info(), animation(), frames(), palette(), paletteWidth(0),
	// Internal state:
	m_liveFile(NULL)
{
	;
}
core::BpdLoader::~BpdLoader()
{
	// Close any live file:
	if (m_liveFile != NULL) {
		fclose(m_liveFile);
		m_liveFile = NULL;
	}
}

bool core::BpdLoader::LoadBpd ( const char* n_resourcename )
{
	// Close any live file:
	if (m_liveFile != NULL) {
		fclose(m_liveFile);
		m_liveFile = NULL;
	}

	// Create the resource name
	arstring256 image_rezname  (n_resourcename);
	std::string image_filename = image_rezname;
	{
		arstring256 file_extension = core::utils::string::GetFileExtension(image_rezname);
		std::string raw_filename;

		core::utils::string::ToLower(file_extension, file_extension.length());
		if (file_extension.compare(""))
		{
			image_filename += ".bpd";
		}
		else
		{	// Remove the extension
			raw_filename = image_rezname;
			image_rezname = raw_filename.substr(0, raw_filename.length() - (file_extension.length() + 1)).c_str();
		}

		const char* const image_extensions[] = {
			".png", ".jpg", ".jpeg", ".gif", ".tga", ".bmp"
		};
		const size_t image_extensions_len = sizeof(image_extensions) / sizeof(const char* const);

		// Loop through and try to find the matching filename:
		bool raw_exists = false;
		for (size_t i = 0; i < image_extensions_len; ++i)
		{
			raw_filename = image_rezname + image_extensions[i];
			// Find the file to source data from:
			if (core::Resources::MakePathTo(raw_filename.c_str(), raw_filename))
			{
				raw_exists = true;
				break;
			}
		}

		// Convert file
		if (raw_exists)
		{
			if (core::Converter::ConvertFile(raw_filename.c_str()) == false)
			{
				debug::Console->PrintError( "BpdLoader::LoadBpd : Error occurred in core::Converter::ConvertFile call\n" );
			}
		}

		// Select the BPD filename after conversion:
		image_filename = image_rezname + ".bpd";

		// Find the file to open...
		if (!core::Resources::MakePathTo(image_filename.c_str(), image_filename))
		{
			debug::Console->PrintError( "BpdLoader::LoadBpd : Could not find image file in the resources.\n" );
			return false;
		}
	}

	// Open the new file:
	m_liveFile = fopen(image_filename.c_str(), "rb");
	if (m_liveFile != NULL)
	{
		return loadBpdCommon();
	}

	return false;
}

//	LoadBpd() : Continues loading.
// Continues to load the live BPD file initially opened with LoadBpd.
bool core::BpdLoader::LoadBpd ( void )
{
	if (m_liveFile != NULL)
	{
		return loadBpdCommon();
	}
	return false;
}

//	LoadBpdCommon() : loads BPD file
bool core::BpdLoader::loadBpdCommon ( void )
{
	fseek(m_liveFile, 0, SEEK_SET);

	bool read_header = false;

	//===============================
	// Read in the header

	textureFmtHeader header;
	if (!m_loadOnlySuperlow)
	{
		fread(&header, sizeof(header), 1, m_liveFile);
		read_header = true; 

		if (strcmp(header.head, kTextureFormat_Header) != 0)
		{
			throw core::CorruptedDataException();
			return false;
		}

		// Save new image info
		if (m_loadImageInfo)
		{
			info.width	= header.width;
			info.height	= header.height;
			info.depth	= header.depth;
			info.levels	= header.levels;
		}

		// Check format
		if ((header.flags & 0x000000FF) != IMG_FORMAT_RGBA8)
		{
			debug::Console->PrintError("BpdLoader::loadBpdCommon : unsupported pixel format. only rgba8 supported at this time\n");
			throw core::YouSuckException();
			return false;
		}
	}

	// Based on that data, read in the other segments

	//===============================
	// Read in the superlow (doesnt need the header)

	if (m_buffer_Superlow)
	{
		fseek(m_liveFile, sizeof(textureFmtHeader), SEEK_SET);
		fread(m_buffer_Superlow, sizeof(gfx::arPixel) * kTextureFormat_SuperlowSize, 1, m_liveFile);
	}

	//===============================
	// Read in texture levels

	if (read_header)
	{
		for (int i = 0; i < header.levels; ++i)
		{
			if (m_buffer_Mipmaps[i] && (m_loadMipmapMask & (1 << i)))
			{
				textureFmtLevel levelInfo;

				// Read in the level info first
				fseek(m_liveFile, header.levelsOffset + sizeof(textureFmtLevel) * i, SEEK_SET);
				fread(&levelInfo, sizeof(levelInfo), 1, m_liveFile);
			
				// Ensure data is correct
				if (strcmp(levelInfo.head, kTextureFormat_HeadLevel) != 0)
				{
					throw core::CorruptedDataException();
					return false;
				}
				if (levelInfo.level != i)
				{
					debug::Console->PrintError( "BpdLoader::loadBpdCommon : Mismatch in mipmap ids, wanted %d got %d.\n", i, levelInfo.level );
					return false;
				}

				// Seek to the actual level data
				fseek(m_liveFile, levelInfo.offset, SEEK_SET);

				// Read in the data to a temp buffer
				uchar* t_sideBuffer = new uchar [levelInfo.size];
				fread(t_sideBuffer, levelInfo.size, 1, m_liveFile);
			
				// Decompress the data directly into target pointer:
				unsigned long t_effectiveWidth	= std::max<unsigned long>(1, header.width / math::exp2(i));
				unsigned long t_effectiveHeight	= std::max<unsigned long>(1, header.height / math::exp2(i));
				unsigned long t_mipmapByteCount	= sizeof(gfx::arPixel) * t_effectiveWidth * t_effectiveHeight;
				int z_result = uncompress( (uchar*)m_buffer_Mipmaps[i], &t_mipmapByteCount, (uchar*)t_sideBuffer, levelInfo.size );

				// Delete the side buffer
				delete [] t_sideBuffer;

				// Check decompress result
				switch( z_result )
				{
				case Z_OK:
					break;
				case Z_MEM_ERROR:
					debug::Console->PrintError("BpdLoader::loadBpdCommon : zlib : out of memory\n");
					break;
				case Z_BUF_ERROR:
					debug::Console->PrintError("BpdLoader::loadBpdCommon : zlib : output buffer wasn't large enough\n");
					break;
				case Z_DATA_ERROR:
					debug::Console->PrintError("BpdLoader::loadBpdCommon : zlib : corrupted data\n");
					break;
				}
			}
		}
	}

	//===============================
	// Read in animation info

	if (m_loadAnimation && read_header && header.animationOffset != 0)
	{
		fseek(m_liveFile, header.animationOffset, SEEK_SET);
		
		// Read in animation header
		textureFmtAnimation animation;
		fread(&animation, sizeof(animation), 1, m_liveFile);

		// Ensure data is correct
		if (strcmp(animation.head, kTextureFormat_HeadAnimation) != 0)
		{
			throw core::CorruptedDataException();
			return false;
		}

		// Set data
		this->animation.framecount	= animation.frames;
		this->animation.xdivs		= animation.xdivs;
		this->animation.ydivs		= animation.ydivs;
		this->animation.framerate	= (animation.framerate == 0) ? 60 : animation.framerate;

		// Read in all the frames too
		frames.resize(animation.frames);
		fread(frames.data(), sizeof(textureFmtFrame) * animation.frames, 1, m_liveFile);
	}

	//===============================
	// Read in palette info

	if (m_loadPalette && read_header && header.paletteOffset != 0)
	{
		fseek(m_liveFile, header.paletteOffset, SEEK_SET);

		// Read in palette header
		textureFmtPalette palette_info;
		fread(&palette_info, sizeof(palette_info), 1, m_liveFile);

		// Ensure data is correct
		if (strcmp(palette_info.head, kTextureFormat_HeadPalette) != 0)
		{
			throw core::CorruptedDataException();
			return false;
		}

		// Set data
		paletteWidth = palette_info.depth;

		// Read in entire palette
		palette.resize(palette_info.rows * palette_info.depth);
		fread(palette.data(), sizeof(gfx::arPixel) * palette_info.rows * palette_info.depth, 1, m_liveFile);
	}

	return true;
}


core::BpdWriter::BpdWriter()
	: m_generateMipmaps(false), m_writeAnimation(false), m_convertAndEmbedPalette(false),
	// Inputs:
	rawImage(NULL), mipmaps(), mipmapCount(0), palette(NULL), paletteRows(0), paletteDepth(0),
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

	return false;
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
	l_header.flags		|= IMG_FORMAT_RGBA8 & 0x000000FF;
	// Byte 1: Transparency & load modes
	l_header.flags		|= (ALPHA_LOAD_MODE_DEFAULT << 8) & 0x0000FF00;
	// Byte 2: Type of texture (animated, paletted?)
	l_header.flags		|= ((m_writeAnimation ? 0x01 : 0) << 16) & 0x00FF0000;
	l_header.flags		|= ((m_offsetPalette  ? 0x02 : 0) << 16) & 0x00FF0000;

	l_header.width		= info.width;
	l_header.height		= info.height;
	l_header.depth		= info.depth;
	l_header.levels		= (uint16_t)m_levels.size();

	l_header.levelsOffset	= m_offsetLevels;
	l_header.animationOffset= m_offsetAnimation;
	l_header.paletteOffset	= m_offsetPalette;

	// Write the header:
	mappedfile->SeekTo(0);
	mappedfile->WriteBuffer(&l_header, sizeof(textureFmtHeader));

	return true;
}

//	writeSuperlow() : generates & writes the superlow texture variant
bool core::BpdWriter::writeSuperlow ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	gfx::arPixel* lowQuality = new gfx::arPixel [kTextureFormat_SuperlowSize];
	uint32_t	aggregate_r, aggregate_g, aggregate_b, aggregate_a;
	uint32_t	set_w, set_h;
	uint32_t	pixelIndex;
	set_w = std::max<uint>( info.width / kTextureFormat_SuperlowWidth, 1 );
	set_h = std::max<uint>( info.height/ kTextureFormat_SuperlowWidth, 1 );
	for ( uint x = 0; x < kTextureFormat_SuperlowWidth; x += 1 )
	{
		for ( uint y = 0; y < kTextureFormat_SuperlowWidth; y += 1 )
		{
			aggregate_r = 0;
			aggregate_g = 0;
			aggregate_b = 0;
			aggregate_a = 0;
			for ( uint sx = set_w * x; sx < set_w * (x + 1); sx += 1 )
			{
				for ( uint sy = set_h * y; sy < set_h * (y + 1); sy += 1 )
				{
					pixelIndex = sx + sy * info.width;
					if ( set_w != 1 ) {
						pixelIndex = sx;
					}
					else {
						pixelIndex = x / (kTextureFormat_SuperlowWidth / info.width);
					}
					if ( set_h != 1 ) {
						pixelIndex += sy * info.width;
					}
					else {
						pixelIndex += (y / (kTextureFormat_SuperlowWidth / info.height)) * info.width;
					}
					pixelIndex = std::min<uint32_t>( pixelIndex, (uint32_t)(info.width * info.height) - 1 );
					aggregate_r += rawImage[pixelIndex].r;
					aggregate_g += rawImage[pixelIndex].g;
					aggregate_b += rawImage[pixelIndex].b;
					aggregate_a += rawImage[pixelIndex].a;
				}
			}
			aggregate_r /= set_w*set_h;
			aggregate_g /= set_w*set_h;
			aggregate_b /= set_w*set_h;
			aggregate_a /= set_w*set_h;

			lowQuality[x+y*kTextureFormat_SuperlowWidth].r = std::min<uint>( aggregate_r, 255 );
			lowQuality[x+y*kTextureFormat_SuperlowWidth].g = std::min<uint>( aggregate_g, 255 );
			lowQuality[x+y*kTextureFormat_SuperlowWidth].b = std::min<uint>( aggregate_b, 255 );
			lowQuality[x+y*kTextureFormat_SuperlowWidth].a = std::min<uint>( aggregate_a, 255 );
			/*if ( n_inputimg_info->internalFormat == RGB8 ) {
				lowQuality[x+y*16].a = 255;
			}*/
		}
	}

	// Write out the low-quality guys
	mappedfile->WriteBuffer(lowQuality, sizeof(gfx::arPixel) * kTextureFormat_SuperlowSize);

	// Remove the LQ data
	delete [] lowQuality;

	return true;
}

bool core::BpdWriter::writeLevelData ( void )
{
	CMappedBinaryFile* mappedfile = (CMappedBinaryFile*)m_file;

	// Level stopper: defines the smallest mipmap size. Set to 3 to stop at 16x16. Set to 4 for 32x32. -1 for 1x1.
	const int kLevelStopper = 3;
	uint16_t level_count = (uint16_t) std::max<int>( 1, math::log2( std::min<uint>(info.width, info.height) ) - kLevelStopper ); 

	// Resize levels
	m_levels.resize(level_count);
	// Write all the level info out as a spacer
	m_offsetLevels = (uint32_t)mappedfile->GetCursor();
	mappedfile->WriteBuffer(m_levels.data(), sizeof(textureFmtLevel) * m_levels.size());

	// Now, allocate the compression and conversion buffers:
	gfx::arPixel*	imageDataResample = m_generateMipmaps ? new gfx::arPixel [info.width * info.height] : NULL;
	uint32_t		compressBufferLen = compressBound(sizeof(gfx::arPixel) * info.width * info.height);
	uchar*			compressBuffer = new uchar [compressBufferLen + 1];

	// Write the levels:
	for (int level = level_count - 1; level >= 0; --level) 
	{	// Looping from smallest level to the raw:
		uint32_t	t_blocks = math::exp2(level);
		uint32_t	t_width  = info.width  / t_blocks;
		uint32_t	t_height = info.height / t_blocks;

		if (m_generateMipmaps)
		{
			// Downsample the image
			uint32_t	aggregate_r, aggregate_g, aggregate_b, aggregate_a;
			uint32_t	pixelTarget;
			for ( uint x = 0; x < t_width; x += 1 )
			{
				for ( uint y = 0; y < t_height; y += 1 )
				{
					aggregate_r = 0;
					aggregate_g = 0;
					aggregate_b = 0;
					aggregate_a = 0;
					for ( uint sx = 0; sx < t_blocks; sx += 1 )
					{
						for ( uint sy = 0; sy < t_blocks; sy += 1 )
						{
							pixelTarget = (x * t_blocks + sx) + (y * t_blocks + sy) * info.width;
							aggregate_r += rawImage[pixelTarget].r;
							aggregate_g += rawImage[pixelTarget].g;
							aggregate_b += rawImage[pixelTarget].b;
							aggregate_a += rawImage[pixelTarget].a;
						}
					}
					aggregate_r /= sqr(t_blocks);
					aggregate_g /= sqr(t_blocks);
					aggregate_b /= sqr(t_blocks);
					aggregate_a /= sqr(t_blocks);

					pixelTarget = x+y*t_width;
					imageDataResample[pixelTarget].r = std::min<uint>( aggregate_r, 255 );
					imageDataResample[pixelTarget].g = std::min<uint>( aggregate_g, 255 );
					imageDataResample[pixelTarget].b = std::min<uint>( aggregate_b, 255 );
					imageDataResample[pixelTarget].a = std::min<uint>( aggregate_a, 255 );
					/*if ( info.internalFormat == RGB8 ) {
						imageDataResample[pixelTarget].a = 255;
					}*/
				}
			}
		}

		// Compress the data into side buffer
		unsigned long compressedSize = compressBufferLen;
		int z_result = compress(
			compressBuffer, &compressedSize,
			(uchar*)(m_generateMipmaps ? imageDataResample : mipmaps[level]),
			sizeof(gfx::arPixel) * t_width * t_height);

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
	delete[] imageDataResample;
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
