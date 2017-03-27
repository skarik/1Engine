
#ifndef _C_TEXTURE_LOADER_H_
#define _C_TEXTURE_LOADER_H_

#include <string>
#include <stdint.h>

#include "renderer/types/pixelFormat.h"
#include "renderer/types/textureStructures.h"

//Texture Converter: In-engine program.
//Bit Pixel Data (BPD)

namespace Textures
{
	//HEADER~
	struct tbpdHeader
	{
		char		head[4];	// Always "BPD\0"
		uint16_t	version[2];		// version
		//	CONVERSION TIME
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

	enum
	{
		IMG_FORMAT_RGBA8	=0x00,	//RGBA8 (Default)
		IMG_FORMAT_DXT3		=0x01,	//DXT3 compression
		IMG_FORMAT_DXT5		=0x02,	//DXT5 compression
		IMG_FORMAT_RGBA16	=0x03,	//HDR Integer
		IMG_FORMAT_RGBA16F	=0x04,	//HDR Floating Point
		IMG_FORMAT_PALLETTE	=0x05	//Palletted GIF
	};

	enum alphaLoadMode_t
	{
		ALPHA_LOAD_MODE_DEFAULT	=0x00,
		ALPHA_LOAD_MODE_INDEXED	=0x01,
		ALPHA_LOAD_MODE_KEYED	=0x02
	};

	//LEVELN~
	struct tbpdLevel
	{
		char		head [4];	// Always "TEX\0"
		uint32_t	size;		// compressed size in bytes
		uint32_t	offset;		// byte offset of where data is located in the file
		char		padding0[4];// 16 byte aligned
	};


	// == FUNCTIONS ==
	// Converts a given file
	RENDER_API void ConvertFile ( const std::string& n_inputfile, const std::string& n_outputfile, const bool n_override=false );
	// Converts given bitmap data
	RENDER_API void ConvertData ( const pixel_t* n_inputimg, const timgInfo* n_inputimg_info, const std::string& n_outputfile, const uint64_t n_timewrite=0 );

	// Given a file, will try to use the proper converter
	RENDER_API pixel_t* LoadRawImageData ( const std::string& n_inputfile, timgInfo& o_info );

	// Creates a default XOR texture
	RENDER_API pixel_t* loadDefault ( timgInfo& o_info );

	// Actual conversions
	RENDER_API pixel_t* loadTGA ( const std::string& n_inputfile, timgInfo& o_info );
	RENDER_API pixel_t* loadJPG ( const std::string& n_inputfile, timgInfo& o_info );
	RENDER_API pixel_t* loadPNG ( const std::string& n_inputfile, timgInfo& o_info );
	RENDER_API pixel_t* loadBPD ( const std::string& n_inputfile, timgInfo& o_info, const int level );

};


#endif//_C_TEXTURE_LOADER_H_