#include "TrConverterJPG.h"

#include "core-ext/system/io/assets/TextureIO.h"
#include "core-ext/system/io/assets/texloader/jpg.hpp"
#include "core/debug/console.h"

bool TrConverterJPG::Convert(const char* inputFilename, const char* outputFilename)
{
	core::gfx::tex::arImageInfo imageInfo = {};
	
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

		// Write the new BPD to the disk.
		return writer.WriteBpd(outputFilename);
	}
	return false;
}