#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>

#include "core/math/Math.h"
#include "core/types/types.h"
#include "core/system/io/FileUtils.h"
#include "core-ext/system/io/Resources.h"
//#include "CTexture.h" // used for RGBA8 enum
#include "renderer/types/pixelFormat.h"
#include "renderer/types/textureFormats.h"
#include "TextureLoader.h"

#include "zlib/zlib.h" // used to compress data


using std::string;

#define BPD_VERSION_MAX 1
#define BPD_VERSION_MIN 7

// Convert a texture file into the engine's format
void Textures::ConvertFile ( const std::string& n_inputfile, const std::string& n_outputfile, const bool n_override )
{
	bool t_performConvert = false;
	uint64_t t_currentTime;
	FILE*	t_outFile = NULL;
	
	string t_inputName = IO::FilenameStandardize( n_inputfile );
	if ( !IO::FileExists( t_inputName ) ) {
		return; // Skip if no file to convert
	}
	string t_outputName = IO::FilenameStandardize( n_outputfile );
	//string t_outputFile = t_inputFile.substr( 0, t_inputFile.find_last_of( "." ) ) + ".bpd";

	// Get the time of the input file
	struct stat f_buf;
	stat( t_inputName.c_str(), &f_buf );
	t_currentTime = (uint64_t)f_buf.st_mtime;

	// If output file doesn't exist, perform convert
	if ( !IO::FileExists( t_outputName ) )
	{
		t_performConvert = true;
		//t_outFile = fopen( t_outputName.c_str(), "wb+" );
	}
	else
	{
		// Open up the output file to compare time
		t_outFile = fopen( t_outputName.c_str(), "rb+" );
		
		// Header
		tbpdHeader bpdHeader;
		fread( &bpdHeader, sizeof(tbpdHeader), 1, t_outFile );
		
		// Compare the times
		if ( bpdHeader.version[0] < BPD_VERSION_MAX || ( bpdHeader.version[0] == BPD_VERSION_MAX && bpdHeader.version[1] < BPD_VERSION_MIN ) || bpdHeader.datetime < t_currentTime )
		{
			t_performConvert = true;
			fclose( t_outFile );
			IO::ClearFile( t_outputName.c_str() );
		}
	}

	// Now, actually perform the file conversion process.
	if ( t_performConvert )
	{
		t_outFile = fopen( t_outputName.c_str(), "wb+" );

		// Print pause info
		std::cout << "Converting an image file. This will take but a moment!" << std::endl;

		// Load in the texture file
		timgInfo imageInfo;
		tPixel* imageData = LoadRawImageData( t_inputName, imageInfo );

		// Go to beginning of output file
		fseek( t_outFile, 0, SEEK_SET );

		// Create header
		tbpdHeader bpdHeader;
		bpdHeader.head[0] = 'B';
		bpdHeader.head[1] = 'P';
		bpdHeader.head[2] = 'D';
		bpdHeader.head[3] = 0;
		bpdHeader.version[0] = BPD_VERSION_MAX;
		bpdHeader.version[1] = BPD_VERSION_MIN;
		bpdHeader.datetime	= t_currentTime;
		bpdHeader.flags		= IMG_FORMAT_RGBA8;
		bpdHeader.width		= imageInfo.width;
		bpdHeader.height	= imageInfo.height;
		bpdHeader.levels	= (uint16_t) std::max<int>( 1, Math.log2( std::min<uint>(imageInfo.width,imageInfo.height) )-3 ); // -3 for 16, -4 for 32
		// Write the header out
		fwrite( &bpdHeader, sizeof(tbpdHeader), 1, t_outFile );

		// Write low quality image
		{
			tPixel* lowQuality = new tPixel [16*16];
			uint32_t	aggregate_r, aggregate_g, aggregate_b, aggregate_a;
			uint32_t	set_w, set_h;
			uint32_t	pixelIndex;
			set_w = std::max<uint>( imageInfo.width/16, 1 );
			set_h = std::max<uint>( imageInfo.height/16, 1 );
			for ( uint x = 0; x < 16; x += 1 )
			{
				for ( uint y = 0; y < 16; y += 1 )
				{
					aggregate_r = 0;
					aggregate_g = 0;
					aggregate_b = 0;
					aggregate_a = 0;
					for ( uint sx = set_w*x; sx < set_w*(x+1); sx += 1 )
					{
						for ( uint sy = set_h*y; sy < set_h*(y+1); sy += 1 )
						{
							pixelIndex = sx+sy*imageInfo.width;
							if ( set_w != 1 ) {
								pixelIndex = sx;
							}
							else {
								pixelIndex = x / (16/imageInfo.width);
							}
							if ( set_h != 1 ) {
								pixelIndex += sy*imageInfo.width;
							}
							else {
								pixelIndex += (y / (16/imageInfo.height))*imageInfo.width;
							}
							aggregate_r += imageData[pixelIndex].r;
							aggregate_g += imageData[pixelIndex].g;
							aggregate_b += imageData[pixelIndex].b;
							aggregate_a += imageData[pixelIndex].a;
						}
					}
					aggregate_r /= set_w*set_h;
					aggregate_g /= set_w*set_h;
					aggregate_b /= set_w*set_h;
					aggregate_a /= set_w*set_h;

					lowQuality[x+y*16].r = std::min<uint>( aggregate_r, 255 );
					lowQuality[x+y*16].g = std::min<uint>( aggregate_g, 255 );
					lowQuality[x+y*16].b = std::min<uint>( aggregate_b, 255 );
					lowQuality[x+y*16].a = std::min<uint>( aggregate_a, 255 );
					if ( imageInfo.internalFormat == RGB8 ) {
						lowQuality[x+y*16].a = 255;
					}
				}
			}
			// Write out the low-quality guys
			fwrite( lowQuality, sizeof(tPixel)*16*16, 1, t_outFile );
			// Remove the LQ data
			delete [] lowQuality;
		}

		// Write the level data
		uint64_t t_levelDataPos = ftell( t_outFile );
		uint64_t *t_levelDataOffsets = new uint64_t [bpdHeader.levels];
		{
			for ( uint i = 0; i < bpdHeader.levels; i += 1 )
			{
				int t_level = (bpdHeader.levels-i)-1; // Loop from low-res to high-res
				int t_total_offset = 0;
				for ( int j = bpdHeader.levels-1; j > t_level; j -= 1 )
				{
					t_total_offset += (imageInfo.width*imageInfo.height) / Math.exp2(j*2);
				}

				tbpdLevel bpdLevel;
				bpdLevel.head[0] = 'T';
				bpdLevel.head[1] = 'E';
				bpdLevel.head[2] = 'X';
				bpdLevel.head[3] = 0;
				bpdLevel.offset = sizeof(tbpdHeader)				// Header size
					+( sizeof(tPixel)*16*16 )				// Size of LoRes
					+( sizeof(tbpdLevel)*bpdHeader.levels )			// All the level data
					+( t_total_offset*sizeof(tPixel) );	// Offset of the level
				// size is (fullsize / (2^level) ) where level is 0 to N
				bpdLevel.size = ((imageInfo.width*imageInfo.height) / Math.exp2(t_level*2))*sizeof(tPixel);

				// Write out the level
				fwrite( &bpdLevel, sizeof(tbpdLevel), 1, t_outFile );
			}
		}

		// Write the higher resolution data (all mipmap levels)
		{
			for ( uint i = 0; i < bpdHeader.levels; i += 1 )
			{
				int t_level = (bpdHeader.levels-i)-1; // Loop from low-res to high-res
				uint32_t	t_blocks = Math.exp2(t_level);
				uint32_t	t_width = imageInfo.width / t_blocks;
				uint32_t	t_height = imageInfo.height / t_blocks;

				tPixel* imageDataResample = new tPixel [t_width*t_height];
				uint32_t t_compressBufferSize = compressBound( t_width*t_height*4 ); //(int) ( t_width*t_height*1.4 );
				uchar* imageDataCompressed = new uchar [t_compressBufferSize+1];

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
								pixelTarget = (x*t_blocks+sx) + (y*t_blocks+sy)*imageInfo.width;
								aggregate_r += imageData[pixelTarget].r;
								aggregate_g += imageData[pixelTarget].g;
								aggregate_b += imageData[pixelTarget].b;
								aggregate_a += imageData[pixelTarget].a;
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
						if ( imageInfo.internalFormat == RGB8 ) {
							imageDataResample[pixelTarget].a = 255;
						}
					}
				}

				// Write the image to the file
				//fwrite( imageDataResample, sizeof(Textures::tPixel)*t_width*t_height, 1, t_outFile );

				{
					// Compress the data into side buffer
					uint32_t t_compressedSize = t_compressBufferSize;
					memcpy( imageDataCompressed, imageDataResample, t_width*t_height*4 );
					int z_result = compress( (uchar*)imageDataCompressed, (unsigned long*)&t_compressedSize, (uchar*)imageDataResample, t_width*t_height*4 );
					// Check the result
					switch( z_result )
					{
					case Z_OK: break;
					case Z_MEM_ERROR:
						printf("Textures::ConvertFile >> ZLIB >> out of memory\n");
						throw std::out_of_range("Out of memory");
						break;
					case Z_BUF_ERROR:
						printf("Textures::ConvertFile >> ZLIB >> output buffer wasn't large enough!\n");
						throw std::out_of_range("Out of space");
						break;
					}
					// Save the current offset
					t_levelDataOffsets[i] = ftell( t_outFile );

					// Write the compressed image to the file
					fwrite( &t_compressedSize, sizeof(uint32_t), 1, t_outFile ); //32 bit int of size
					fwrite( imageDataCompressed, t_compressedSize, 1, t_outFile ); // the compressed image
				}

				// Remove the LQ data
				delete [] imageDataResample;
				delete [] imageDataCompressed;
			}
		}

		// Rewrite the offsets
		fseek( t_outFile, (long)t_levelDataPos, SEEK_SET );
		{
			for ( uint i = 0; i < bpdHeader.levels; i += 1 )
			{
				int t_level = (bpdHeader.levels-i)-1; // Loop from low-res to high-res

				tbpdLevel bpdLevel;
				bpdLevel.head[0] = 'T';
				bpdLevel.head[1] = 'E';
				bpdLevel.head[2] = 'X';
				bpdLevel.head[3] = 0;
				bpdLevel.offset = (uint32_t)t_levelDataOffsets[i]; // Use the saved offsets
				// size is (fullsize / (2^level) ) where level is 0 to N
				bpdLevel.size = ((imageInfo.width*imageInfo.height) / Math.exp2(t_level*2))*sizeof(tPixel);

				// Write out the level
				fwrite( &bpdLevel, sizeof(tbpdLevel), 1, t_outFile );
			}
		}
		
		// Clear up the offset data
		delete [] t_levelDataOffsets;

		// Clear up the texture data
		delete [] imageData;
	}
	fclose( t_outFile );
};