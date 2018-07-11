//===============================================================================================//
//
//	TextureIO.h
//
// Definitions for the Bit Pixel Data (BPD) format. 
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
	static const int	kTextureFormat_VersionMinor	= 0;

	static const char*	kTextureFormat_HeadLevel	= "TEX\0";
	static const char*	kTextureFormat_HeadAnimation= "ANM\0";
	static const char*	kTextureFormat_HeadPalette	= "PAL\0";
	static const char*	kTextureFormat_HeadMisc		= "MSC\0";

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
		uint32_t	postLevelsOffset;
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

	static_assert(sizeof(textureFmtHeader)		== sizeof(uint32_t)*10,	"Invalid structure size");
	static_assert(sizeof(textureFmtLevel)		== sizeof(uint32_t)*4,	"Invalid structure size");
	static_assert(sizeof(textureFmtAnimation)	== sizeof(uint32_t)*4,	"Invalid structure size");
	static_assert(sizeof(textureFmtFrame)		== sizeof(uint32_t)*1,	"Invalid structure size");

	//	class BpdLoader
	// Loads up a BPD and given levels based on configuration.
	class BpdLoader
	{
	public:
		CORE_API explicit BpdLoader ( void );
		CORE_API		 ~BpdLoader ( void );

		//	LoadBpd ( resource name )
		// Attempts to load BPD with given resource name.
		CORE_API bool LoadBpd ( const char* n_resourcename );

	public:
		//	Load options:

		// Do we want to load the superlow?
		bool			m_loadSuperlow;
		// Bitmask of which mipmap levels to load. To only load raw, set it to 0x01.
		uint16_t		m_loadMipmapMask;
		// Should the palette be loaded (if there is one?)
		bool			m_loadPalette;
		// Should the memory allocated for the textures be freed when done?
		bool			m_keepMipmapsLive;
		bool			m_keepPaletteLive;

		//	Loaded data:

		gfx::arPixel*					mipmaps [16];
		int								mipmapCount;
		gfx::arPixel*					palette;

		gfx::tex::arImageInfo			info;
		textureFmtAnimation				animation;
		std::vector<textureFmtFrame>	frames;
	};

	//	class BpdWriter
	// Given the input data, will generate mipmaps/palettes, and whatnot for the bpd.
	class BpdWriter
	{
	public:
		CORE_API explicit BpdWriter ( void );
		CORE_API		 ~BpdWriter ( void );

		//	LoadBpd(resource name)
		// Attempts to save new BPD with given resource name. Takes into account input data.
		CORE_API bool WriteBpd ( const char* n_newfilename );

	public:
		//	Save options:

		// Should mipmaps be generated as we save?
		bool			m_generateMipmaps;
		// Controls if image is converted to the the palette given before saved. This will prevent mipmaps from being generated.
		bool			m_convertAndEmbedPalette;
		// 

		//	Save data:

		gfx::arPixel*			rawImage;
		gfx::arPixel*			palette;
		gfx::arPixel*			mipmaps [16];
		int						mipmapCount;

		gfx::tex::arImageInfo	info;
		uint16_t*				frame_times;

		uint64_t				datetime;

	private:
		bool			writeHeader ( void );
		bool			writeLevelData ( void );

		uint32_t		calculateLevelSize ( int );

	private:
		FILE*			m_file;
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

		////	getLoaderByExtension(filename) : Returns needed function loader.
		//// Checks the extension of the filename and returns needed function loader.
		//// Arguments:
		////	n_inputfile: Input filename.
		//// Returns:
		////	arTextureLoaderFn: Loader. NULL if file type is unrecognized.
		//static arTextureLoaderFn getLoaderByExtension ( const char* const n_inputfile )
		//{
		//	arstring256 fileExt (n_inputfile);
		//	::core::utils::string::ToFileExtension(fileExt, sizeof(fileExt));
		//	::core::utils::string::ToLower(fileExt, sizeof(fileExt));

		//	if (fileExt.compare("bpd"))
		//		return loadBPD;
		//	if (fileExt.compare("png"))
		//		return loadPNG;
		//	if (fileExt.compare("jpg") || fileExt.compare("jpeg"))
		//		return loadJPG;
		//	if (fileExt.compare("tga"))
		//		return loadTGA;
		//	if (fileExt.compare("bmp"))
		//		return loadBMP;

		//	return NULL;
		//}
	}
};


#endif//CORE_ASSETS_TEXTURE_IO_H_