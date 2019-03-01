#include "Conversion.h"

#include "core/utils/string.h"
#include "core/system/io/CBinaryFile.h"
#include "core/debug/console.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/Conversion.h"

bool render2d::preprocess::ConvertSprite ( const char* resource_name, rrConvertSpriteResults* convert_results )
{
	std::string resource_albedo = resource_name;
	std::string resource_normals = resource_albedo + "_normals";
	std::string resource_surface = resource_albedo + "_surface";
	std::string resource_illumin = resource_albedo + "_illumin";

	// load up the GAL files first. If that fails, then load up normal images
	bool raw_gal_exists = false;
	std::string raw_filename_albedo;
	{
		raw_filename_albedo = resource_albedo + ".gal";

		if (core::Resources::MakePathTo(raw_filename_albedo.c_str(), raw_filename_albedo))
		{
			// Convert the GAL file.
			if (core::Converter::ConvertFile(raw_filename_albedo.c_str()) == false)
			{
				debug::Console->PrintError( "render2d::preprocess::ConvertSprite : Error occurred in core::Converter::ConvertFile call\n" );
			}
			// Mark we have a GAL so we skip the other image loader
			raw_gal_exists = true;
		}
	}
	// now we load up the normal images
	if (!raw_gal_exists)
	{
		for (int iFile = 0; iFile < 4; ++iFile)
		{
			// Select filename to find files:
			std::string baseFileName;
			if (iFile == 0) baseFileName = resource_albedo;
			else if (iFile == 1) baseFileName = resource_normals;
			else if (iFile == 2) baseFileName = resource_surface;
			else if (iFile == 3) baseFileName = resource_illumin;

			const char* const image_extensions[] = {
				".png", ".jpg", ".jpeg", ".gif", ".tga", ".bmp"
			};
			const size_t image_extensions_len = sizeof(image_extensions) / sizeof(const char* const);

			// Loop through and try to find the matching filename:
			bool raw_exists = false;
			for (size_t i = 0; i < image_extensions_len; ++i)
			{
				raw_filename_albedo = baseFileName + image_extensions[i];
				// Find the file to source data from:
				if (core::Resources::MakePathTo(raw_filename_albedo.c_str(), raw_filename_albedo))
				{
					raw_exists = true;
					break;
				}
			}

			// Convert file
			if (raw_exists)
			{
				if (core::Converter::ConvertFile(raw_filename_albedo.c_str()) == false)
				{
					debug::Console->PrintError( "render2d::preprocess::ConvertSprite : Error occurred in core::Converter::ConvertFile call\n" );
				}
			}
		}
	}

	// Select the BPD filename after conversion:
	if (core::Resources::Exists((resource_albedo + ".bpd")))
		convert_results->hasAlbedo = true;
	if (core::Resources::Exists((resource_normals + ".bpd")))
		convert_results->hasNormals = true;
	if (core::Resources::Exists((resource_surface + ".bpd")))
		convert_results->hasSurface = true;
	if (core::Resources::Exists((resource_illumin + ".bpd")))
		convert_results->hasIllumin = true;
	convert_results->hasAnimation = false;

	// if no albedo exists, then it's not a valid sprite conversion
	if (convert_results->hasAlbedo)
		return true;
	else 
		return false;
}


