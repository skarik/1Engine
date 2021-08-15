#include "TextureIO.h"

#include "core/utils/string.h"
#include "core/system/io/CBinaryFile.h"
#include "core/math/Math.h"
#include "core/debug/console.h"
#include "core/exceptions.h"
#include "core/system/io/CMappedBinaryFile.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/Conversion.h"
#include "core-ext/system/io/Files.h"

#include "zlib/zlib.h"

core::BpdLoader::BpdLoader()
	: m_loadOnlySuperlow(false), m_loadImageInfo(false), m_loadMipmapMask(0), m_loadPalette(false), m_loadAnimation(false),
	// Buffer Outputs:
	m_buffer_Superlow(NULL), m_buffer_Mipmaps(),
	// Outputs:
	format(IMG_FORMAT_INVALID), mipmapCount(0), info(), animation(), frames(), palette(), paletteWidth(0),
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
		std::string raw_filename = image_rezname;

		bool bRawFound = false;
		bool bBpdFound = false;

		core::utils::string::ToLower(file_extension, file_extension.length());
		if (file_extension.length() > 0)
		{	// Remove the extension
			image_rezname = raw_filename.substr(0, raw_filename.length() - (file_extension.length() + 1)).c_str();
		}

		const char* const image_extensions[] = {
			".png", ".jpg", ".jpeg", ".gif", ".tga", ".bmp", ".gal"
		};
		const size_t image_extensions_len = sizeof(image_extensions) / sizeof(const char* const);

		// Loop through and try to find the matching filename:
		for (size_t i = 0; i < image_extensions_len; ++i)
		{
			raw_filename = image_rezname + image_extensions[i];
			// Find the file to source data from:
			if (core::Resources::MakePathTo(raw_filename.c_str(), raw_filename))
			{
				bRawFound = true;
				break;
			}
		}

		// Select the BPD filename:
		image_filename = image_rezname + ".bpd";

		// Check if we have a BPD now
		if (core::Resources::MakePathTo(image_filename.c_str(), image_filename))
		{
			bBpdFound = true;
		}

		// If we have the raw, convert against the 
		if (bRawFound)
		{
			bool bBpdOutOfDate = false;
			bool bBpdCorrupted = false;

			if (bBpdFound)
			{
				// Read in the header
				m_liveFile = fopen(image_filename.c_str(), "rb");
				ARCORE_ASSERT(m_liveFile != NULL);
				textureFmtHeader header;
				fread(&header, sizeof(header), 1, m_liveFile);
				fclose(m_liveFile);

				// Check for correct BPD format & version
				bBpdCorrupted = strcmp(header.head, kTextureFormat_Header) != 0;
				bBpdOutOfDate = header.version[0] < kTextureFormat_VersionMajor || (header.version[0] == kTextureFormat_VersionMajor && header.version[1] < kTextureFormat_VersionMinor);

				// Grab the BPD's age
				uint64 bpd_datetime = header.datetime;

				// Grab the Raw's age
				uint64 raw_datetime = io::file::GetLastWriteTime(raw_filename.c_str());

				// Also check if we're out of date here
				bBpdOutOfDate = bBpdOutOfDate || (raw_datetime > bpd_datetime);
			}

			// Convert file
			if ((!bBpdFound || bBpdOutOfDate || bBpdCorrupted)
				&& core::Converter::ConvertFile(raw_filename.c_str()) == false)
			{
				debug::Console->PrintError( "BpdLoader::LoadBpd : Error occurred in core::Converter::ConvertFile call\n" );
			}

			// Find the file to open post-convert...
			image_filename = image_rezname + ".bpd";
			if (!core::Resources::MakePathTo(image_filename.c_str(), image_filename))
			{
				debug::Console->PrintError( "BpdLoader::LoadBpd : Could not find image file in the resources.\n" );
				return false;
			}
			else
			{
				bBpdFound = true;
			}
		}

		// Check we have a BPD
		if (!bBpdFound)
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
			info.levels	= (uint8_t)std::min<uint16_t>(255, header.levels);	

			format		= (ETextureFormatTypes)(header.flags & 0x000000FF);
		}

		// Check format
		if (format == IMG_FORMAT_DXT3 || format == IMG_FORMAT_DXT5 || format == IMG_FORMAT_ASTC)
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
		uint32_t formatFlags;
		fread(&formatFlags, sizeof(uint32_t), 1, m_liveFile);
		format = (ETextureFormatTypes)(formatFlags & 0xFF);
		fread(m_buffer_Superlow, kTextureFormat_SuperlowByteSize, 1, m_liveFile);

		switch (formatFlags & 0xFF)
		{
		case IMG_FORMAT_RGBA16:
		case IMG_FORMAT_RGBA16F:
			ARCORE_ERROR("Unsupported expansion at this time");
			break;
		}
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
				unsigned long t_mipmapByteCount	= (uint32_t)core::getTextureFormatByteSize(format) * t_effectiveWidth * t_effectiveHeight;
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