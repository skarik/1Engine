//===============================================================================================//
//
//	TextureLoader.h
//
// Definitions for the Bit Pixel Data (BPD) format. 
//
//===============================================================================================//
#ifndef CORE_ASSETS_TEXTURE_LOADER_H_
#define CORE_ASSETS_TEXTURE_LOADER_H_

#include <string>
#include <stdint.h>

#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"

//Texture Converter: In-engine program.
//Bit Pixel Data (BPD)

namespace core
{
	static const char*	kTextureFormat_Header		= "BPD\0";

	static const int	kTextureFormat_VersionMajor	= 1;
	static const int	kTextureFormat_VersionMinor	= 7;

	//HEADER~
	struct textureFmtHeader
	{
		char		head[4];	// Always "BPD\0"
		uint16_t	version[2];	// version
		//	CONVERSION TIME
		// Date of creation/conversion
		uint64_t	datetime;

		//	FLAGS
		// First 8 bits are for texture type
		uint32_t	flags;
		//	SIZE
		uint16_t	width;
		uint16_t	height;
		uint16_t	depth;
		//	MIPMAP LEVELS
		// The smallest width will be
		// width/(2^(levels-1))
		uint16_t	levels;
		//	FRAMES
		uint16_t	frames;
		uint8_t		xdivs;
		uint8_t		ydivs;
	};

	enum ETextureFormatTypes
	{
		IMG_FORMAT_RGBA8	= 0x00,	// RGBA8 (Default)
		IMG_FORMAT_DXT3		= 0x01,	// DXT3 compression
		IMG_FORMAT_DXT5		= 0x02,	// DXT5 compression
		IMG_FORMAT_RGBA16	= 0x03,	// HDR Integer
		IMG_FORMAT_RGBA16F	= 0x04,	// HDR Floating Point
		IMG_FORMAT_PALLETTE	= 0x05	// Palletted GIF
	};

	enum ETextureLoadMode
	{
		ALPHA_LOAD_MODE_DEFAULT	= 0x00,
		ALPHA_LOAD_MODE_INDEXED	= 0x01,
		ALPHA_LOAD_MODE_KEYED	= 0x02
	};

	//LEVELN~
	struct textureFmtLevel
	{
		char		head [4];		// Always "TEX\0"
		uint32_t	size;			// compressed size in bytes
		uint32_t	offset;			// byte offset of where data is located in the file
		char		padding0 [4];	// 16 byte aligned
	};

	static_assert(sizeof(textureFmtHeader)	== sizeof(uint32_t)*8,	"Invalid structure size");
	static_assert(sizeof(textureFmtLevel)	== sizeof(uint32_t)*4,	"Invalid structure size");

	// Converts a given file
	//CORE_API void ConvertFile ( const std::string& n_inputfile, const std::string& n_outputfile, const bool n_override=false );
	
	// Converts given bitmap data
	//CORE_API void ConvertData ( const gfx::arPixel* n_inputimg, const gfx::tex::arImageInfo* n_inputimg_info, const std::string& n_outputfile, const uint64_t n_timewrite=0 );

	//	class BpdLoader
	// Loads up a BPD and given levels based on configuration.
	class BpdLoader
	{
	public:
		CORE_API bool LoadBpd ( const char* n_resourcename );

	};

	//	class BpdWriter
	// Given the input data, will generate mipmaps/palettes, and whatnot for the bpd.
	class BpdWriter
	{
	public:
		CORE_API bool WriteBpd ( const char* n_newfilename );
	};

	namespace texture
	{
		// Given a file, will try to use the proper loader:

		static gfx::arPixel* loadImage ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info )
		{

		}

		// Creates a default XOR texture:

		static gfx::arPixel* genXOR ( gfx::tex::arImageInfo& o_info, const uint16_t width = 64, const uint16_t height = 64 )
		{
			// Set the new texture size
			o_info.width = width;
			o_info.height = height;

			// Set the internal format to just RGB8
			o_info.internalFormat = gfx::tex::kColorFormatRGB8;

			// Create the pixel data
			gfx::arPixel * pData = new gfx::arPixel [ o_info.width * o_info.height ];

			// Create the data
			unsigned int iTarget;
			unsigned short int c;
			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
				{
					iTarget = ix+(iy*o_info.width);
					c = (ix^iy)*4;
					pData[iTarget].r = char(c);
					pData[iTarget].g = char(c);
					pData[iTarget].b = char(c);
					pData[iTarget].a = char(c);
				}
			}
			return pData;
		}

		// Actual conversions:

		static gfx::arPixel* loadTGA ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadBMP ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadJPG ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadPNG ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info );
	}
};


#endif//CORE_ASSETS_TEXTURE_LOADER_H_