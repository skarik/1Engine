//===============================================================================================//
//
//	TextureIO.h
//
// Definitions for the Bit Pixel Data (BPD) format. 
// Changelog at bottom.
//
//===============================================================================================//
#ifndef CORE_ASSETS_TEXTURE_IO_H_
#define CORE_ASSETS_TEXTURE_IO_H_

#include <string>
#include <stdint.h>
#include <vector>

#include "core/containers/arstring.h"
#include "core/utils/string.h"
#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"

namespace core
{
	static const char*	kTextureFormat_Header		= "BPD\0";

	static const int	kTextureFormat_VersionMajor	= 2;
	static const int	kTextureFormat_VersionMinor	= 3;

	static const char*	kTextureFormat_HeadLevel	= "TEX\0";
	static const char*	kTextureFormat_HeadAnimation= "ANM\0";
	static const char*	kTextureFormat_HeadPalette	= "PAL\0";
	static const char*	kTextureFormat_HeadMisc		= "MSC\0";

	static const int	kTextureFormat_SuperlowWidth	= 16;
	static const int	kTextureFormat_SuperlowCount	= 16 * 16;
	static const int	kTextureFormat_SuperlowByteSize	= 4 * 16 * 16;

	// Texture formats. In BPDs, these are read from both the header and the superlow segment.
	// They can be queried from the header.
	enum ETextureFormatTypes
	{
		IMG_FORMAT_RGBA8	= 0x00 | 0x00,	// RGBA8 (Default).
		IMG_FORMAT_RGB8		= 0x00 | 0x10,	// RGB8.
		IMG_FORMAT_RGBA16	= 0x01 | 0x00,	// HDR Integer
		IMG_FORMAT_RGBA16F	= 0x02 | 0x00,	// HDR Floating Point
		IMG_FORMAT_DXT3		= 0x03 | 0x00,	// DXT3 compression
		IMG_FORMAT_DXT5		= 0x03 | 0x10,	// DXT5 compression
		IMG_FORMAT_ASTC		= 0x03 | 0x20,	// DXT5 compression
		IMG_FORMAT_PALLETTE	= 0x04 | 0x00,	// Palletted format. XY8/RG8.

		IMG_FORMAT_INVALID	= 0xFF,
	};

	enum ETextureLoadMode
	{
		ALPHA_LOAD_MODE_DEFAULT	= 0x00,
		ALPHA_LOAD_MODE_INDEXED	= 0x01,
		ALPHA_LOAD_MODE_KEYED	= 0x02
	};

	//	getTextureFormatByteSize(type) : Given a format, returns the byte width of a single texel.
	static size_t getTextureFormatByteSize (ETextureFormatTypes type)
	{
		switch (type)
		{
		case IMG_FORMAT_RGBA8:		return 4;
		case IMG_FORMAT_RGB8:		return 3;
		case IMG_FORMAT_RGBA16:		return 8;
		case IMG_FORMAT_RGBA16F:	return 8;

		case IMG_FORMAT_PALLETTE:	return 2;
		}
		ARCORE_ERROR("Invalid or unsupported type passed.");
		return 0;
	}

	//	getColorFormatFromTextureFormat(type) : Given a texture format, returns a matching GFX format.
	static core::gfx::tex::arColorFormat getColorFormatFromTextureFormat (ETextureFormatTypes type)
	{
		switch (type)
		{
		case IMG_FORMAT_RGBA8:		return core::gfx::tex::kColorFormatRGBA8;
		case IMG_FORMAT_RGB8:		return core::gfx::tex::kColorFormatRGB8;
		case IMG_FORMAT_RGBA16:		return core::gfx::tex::kColorFormatRGBA16;
		case IMG_FORMAT_RGBA16F:	return core::gfx::tex::kColorFormatRGBA16F;

		case IMG_FORMAT_PALLETTE:	return core::gfx::tex::kColorFormatRG8;
		}
		ARCORE_ERROR("Invalid or unsupported type passed.");
		return core::gfx::tex::kColorFormatNone;
	}

	struct textureFmtHeader
	{
	public:
		char		head[4];	// Always "BPD\0"
		uint16_t	version[2];	// version
		//	CONVERSION TIME
		// Date of creation/conversion
		uint64_t	datetime;

		//	FLAGS
		// First 8 bits are for texture type
		uint32_t	flags;

		//	MIPMAP INFO
		// The smallest width will be width/(2^(levels-1))
		uint16_t	levels;

		//	SIZE
		uint16_t	width;
		uint16_t	height;
		uint16_t	depth;

		//	POINTERS
		// byte offset of where level info begins
		uint32_t	levelsOffset;
		// byte offset
		uint32_t	animationOffset;
		// byte offset
		uint32_t	paletteOffset;
		// byte offset of normal data. use the levels information to load mips. currently unused.
		uint32_t	normalsOffset; 
		// byte offset of surface data. use the levels information to load mips. currently unused.
		uint32_t	surfaceOffset;
		// byte offset of illumination data. use the levels information to load mips. currently unused.
		uint32_t	illuminOffset;

		uint32_t _unused0;
		uint32_t _unused1;
		uint32_t _unused2;

	public:
		//void setTextureType
	};

	struct textureFmtLevel
	{
		char		head [4];	// Always "TEX\0"
		//	LEVEL DATA
		uint32_t	size;		// compressed size in bytes
		uint32_t	offset;		// byte offset of where data is located in the file
		uint16_t	level;		// actual level
		uint16_t	base;		// unused
	};

	struct textureFmtAnimation
	{
		char		head[4];	// Always "ANM\0"

		//	FRAMES
		uint16_t	frames;
		uint8_t		xdivs;
		uint8_t		ydivs;

		//	ANIMATION
		uint16_t	framerate;	// Base framerate. If 0, assumes 60.

		uint16_t	unused0[3];
	};
	struct textureFmtFrame
	{
		//	FRAME DATA
		uint16_t	index;
		uint16_t	duration;
	};

	struct textureFmtPalette
	{
		char		head[4];	// Always "PAL\0"
		uint8_t		rows;
		uint8_t		depth;

		uint16_t	unused0;
	};

	static_assert(sizeof(textureFmtHeader)		== sizeof(uint32_t)*16,	"Invalid structure size");
	static_assert(sizeof(textureFmtLevel)		== sizeof(uint32_t)*4,	"Invalid structure size");
	static_assert(sizeof(textureFmtAnimation)	== sizeof(uint32_t)*4,	"Invalid structure size");
	static_assert(sizeof(textureFmtFrame)		== sizeof(uint32_t)*1,	"Invalid structure size");
	static_assert(sizeof(textureFmtPalette)		== sizeof(uint32_t)*2,	"Invalid structure size");

	namespace gfx
	{
		namespace tex
		{
			struct vecRGBA8
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			struct vecRGB8
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
			};
			struct vecXY8
			{
				uint8_t x;
				uint8_t y;
			};
		}
	}

	//	class BpdLoader
	// Loads up a BPD and given levels based on configuration.
	class BpdLoader
	{
	public:
		CORE_API explicit		BpdLoader ( void );
		CORE_API				~BpdLoader ( void );

		//	LoadBpd ( resource name )
		// Attempts to load BPD with given resource name.
		// If the file could not be loaded, will return false.
		CORE_API bool			LoadBpd ( const char* n_resourcename );

		//	LoadBpd() : Continues loading.
		// Continues to load the live BPD file initially opened with LoadBpd.
		CORE_API bool			LoadBpd ( void );

	public:
		//	Load options:

		// Do we want to load the superlow?
		bool			m_loadOnlySuperlow;
		// Load the image info?
		bool			m_loadImageInfo;
		// Bitmask of which mipmap levels to load. To only load raw, set it to 0x01.
		uint16_t		m_loadMipmapMask;
		// Should the palette be loaded (if there is one?)
		bool			m_loadPalette;
		// Load the animation?
		bool			m_loadAnimation;
		// Force a texture format?
		bool			m_forceTextureFormat = false;
		// Forced format?
		core::gfx::tex::arColorFormat
						m_forcedTextureFormat = core::gfx::tex::kColorFormatRGBA8;

		//	Load targets:

		// Superlow load target. If not null, should point to a buffer of at least size kTextureFormat_SuperlowByteSize.
		void*			m_buffer_Superlow;
		// Mipmap load targets. If not null, should point to buffers of correct size for the texture.
		void*			m_buffer_Mipmaps [16];

		//	Load outputs:

		// Loaded data's format
		ETextureFormatTypes
						format;
		// Number of mipmaps.
		uint8_t			mipmapCount;
		gfx::tex::arImageInfo
						info;
		gfx::tex::arAnimationInfo
						animation;
		std::vector<textureFmtFrame>
						frames;
		// Loaded palette. Always 8-bit color.
		std::vector<gfx::arPixel>
						palette;
		uint8_t			paletteWidth;

	protected:
		FILE*			m_liveFile;

		//	loadBpdCommon() : loads BPD file
		bool					loadBpdCommon ( void );
	};

	//	class BpdWriter
	// Given the input data, will generate mipmaps/palettes, and whatnot for the bpd.
	class BpdWriter
	{
	public:
		CORE_API explicit		BpdWriter ( void );
		CORE_API				~BpdWriter ( void );

		//	LoadBpd(resource name)
		// Attempts to save new BPD with given file name.
		CORE_API bool			WriteBpd ( const char* n_newfilename );

	public:
		//	Save options:

		// Should mipmaps be generated as we save?
		bool			m_generateMipmaps;
		// Controls if animation should be written.
		bool			m_writeAnimation;
		// Controls if image is converted to the the palette given before saved. This will prevent mipmaps from being generated.
		bool			m_convertAndEmbedPalette;
		// 

		//	Save data:

		ETextureFormatTypes
						rawImageFormat;
		// Raw image to generate mipmaps from. Must be always set.
		void*			rawImage = nullptr;
		// If mipmaps are not generated, the image data we pull from.
		void*			mipmaps [16];
		int				mipmapCount;
		gfx::arPixel*	palette;
		int				paletteRows;
		int				paletteDepth;

		gfx::tex::arImageInfo
						info;
		gfx::tex::arAnimationInfo
						animationInfo;
		uint16_t*		frame_times;

		uint64_t		datetime;

	private:
		//	writeHeader() : writes header
		bool					writeHeader ( void );
		//	writeSuperlow() : generates & writes the superlow texture variant
		bool					writeSuperlow ( void );
		//	writeLevelData() : writes all the mipmapped levels. possibly generates them.
		bool					writeLevelData ( void );
		//	writeAnimation() : writes animation needed
		bool					writeAnimation ( void );
		//	writePalette() : writes palette needed
		bool					writePalette ( void );
		//	patchHeader() : fixes up the data in the header
		bool					patchHeader( void );
		//	patchLevels() : fixes up the data in the level data
		bool					patchLevels( void );

	private:
		void*			m_file;
		uint32_t		m_offsetLevels;
		std::vector<textureFmtLevel>
						m_levels;
		uint32_t		m_offsetAnimation;
		uint32_t		m_offsetPalette;
	};

	namespace texture
	{
		//	arTextureLoaderFn : Pointer to a loader function
		typedef gfx::arPixel* (*arTextureLoaderFn)( const char* const n_inputfile, gfx::tex::arImageInfo& o_info );

		// Loaders :

		static gfx::arPixel* loadTGA ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadBMP ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadJPG ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadPNG ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info );
		static gfx::arPixel* loadBPD ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info )
			{ return NULL; }

		//	genXOR(info, [width, height]): Generates an XOR texture.
		// Will create an XOR textured, which can be used for debugging or error output.
		// Arguments:
		//	o_info: Output image info needed for GPUW.
		//	[width]: Optional width of the image generated. Default is 64.
		//	[height]: Optional height of the image generated. Default is 64.
		// Returns:
		//	arPixel*: Array of pixels of length (width * height), represents the image.
		static gfx::arPixel* genXOR ( gfx::tex::arImageInfo& o_info, const uint16_t width = 64, const uint16_t height = 64 )
		{
			// Set the new texture size
			o_info.width = width;
			o_info.height = height;

			// Set the internal format to just RGB8
			//o_info.internalFormat = gfx::tex::kColorFormatRGB8;

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
	}
};

//===============================================================================================//
//
// BPD version 2.3
//	• optional mipmap generation
//	• barebones 3d texture support
//	• added texture format field to the header
//	• alpha-blending in mipmap generation
//
//===============================================================================================//

#endif//CORE_ASSETS_TEXTURE_IO_H_