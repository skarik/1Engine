#include "TrConverterJPG.h"

#include "core-ext/system/io/assets/TextureIO.h"
#include "core-ext/system/io/assets/texloader/jpg.hpp"
#include "core/debug/console.h"

bool TrConverterJPG::Convert(const char* inputFilename, const char* outputFilename)
{
	core::gfx::tex::arImageInfo imageInfo = {};
	
	// TODO: replace all this shit with arTextureInfo to get the proper bitdepth

	// Open the PNG and read the data:
	core::gfx::arPixel* pixel = core::texture::loadJPG(inputFilename, imageInfo);
	
	// It loaded properly?
	if (pixel != NULL)
	{
		// Set the data we need to write:
		core::BpdWriter writer;
		writer.m_generateMipmaps = true;
		writer.rawImage = pixel;
		writer.info = imageInfo;
		writer.rawImageFormat = core::IMG_FORMAT_RGBA8;

		// Get the time of the input file
		struct stat f_buf;
		stat(inputFilename, &f_buf);
		writer.datetime = (uint64_t)f_buf.st_mtime;

		// Write the new BPD to the disk.
		return writer.WriteBpd(outputFilename);
	}
	return false;
}