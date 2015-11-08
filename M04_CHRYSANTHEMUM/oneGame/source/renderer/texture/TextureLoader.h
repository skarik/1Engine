
#ifndef _C_TEXTURE_LOADER_H_
#define _C_TEXTURE_LOADER_H_

#include <string>
#include <stdint.h>

#include "renderer/types/pixelFormat.h"

//Texture Converter: In-engine program.
//Bit Pixel Data (BPD)

namespace Textures
{
	//HEADER~
	struct tbpdHeader
	{
		char		head[4];	// Always "BPD\0"
		uint16_t	version[2];		// version
		uint64_t	datetime;
		//	FLAGS
		// First 8 bits are for texture type
		uint32_t	flags;
		uint16_t	width;
		uint16_t	height;
		uint16_t	depth;
		uint16_t	levels;		// (mipmap levels)
		// The smallest width will be
		// width/(2^(levels-1))
		uint16_t	frames;
		char 		padding0[2];	// 32 byte aligned
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

	//LEVELN~
	struct tbpdLevel
	{
		char		head [4];	// Always "TEX\0"
		uint32_t	size;		// compressed size in bytes
		uint32_t	offset;		// byte offset of where data is located in the file
		char		padding0[4];// 16 byte aligned
	};


	// IMAGE INFO
	struct timgInfo
	{
		uint16_t	width;
		uint16_t	height;

		uint32_t	internalFormat;
	};

	// == FUNCTIONS ==
	// Converts a given file
	RENDER_API void ConvertFile ( const std::string& n_inputfile, const std::string& n_outputfile, const bool n_override=false );

	// Given a file, will try to use the proper converter
	RENDER_API tPixel* LoadRawImageData ( const std::string& n_inputfile, timgInfo& o_info );

	// Creates a default XOR texture
	RENDER_API tPixel* loadDefault ( timgInfo& o_info );

	// Actual conversions
	RENDER_API tPixel* loadTGA ( const std::string& n_inputfile, timgInfo& o_info );
	RENDER_API tPixel* loadJPG ( const std::string& n_inputfile, timgInfo& o_info );
	RENDER_API tPixel* loadPNG ( const std::string& n_inputfile, timgInfo& o_info );
	RENDER_API tPixel* loadBPD ( const std::string& n_inputfile, timgInfo& o_info, const int level );

};


#endif//_C_TEXTURE_LOADER_H_