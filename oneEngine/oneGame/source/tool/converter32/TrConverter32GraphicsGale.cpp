#include "tool/converter32/TrConverter32GraphicsGale.h"

#include "core/os.h"
#include "core/utils/string.h"
#include "core-ext/system/io/FileUtils.h"

#include <vector>

enum trImageDataUsage
{
	kTrImageDataUsageColor,
	kTrImageDataUsagePalette,
	kTrImageDataUsageNormal,
	kTrImageDataUsageSurface,
	kTrImageDataUsageGeneral,
};

struct trImageDataPair
{
	arstring256 outputSuffix;
	arstring256 outputExt;
	trImageDataUsage usage;
};

bool TrConverter32GraphicsGale::Convert(const char* inputFilename, const char* outputFilename) 
{
	arstring256 l_inputFileStem (inputFilename);
	core::utils::string::ToFileStemLeaf(l_inputFileStem);

	arstring256 l_outputFileStem (outputFilename);
	core::utils::string::ToFileStemLeaf(l_outputFileStem);

	std::vector<trImageDataPair> read_data;

	// GAL's are unique in that they have layer data, so we have to check for all the named layers that are
	// recognized by the engine (normal, surface, and etc.)

	// Also check for a palette file:


	// If we have no palette file, we save in the same way we would save a JPEG.
	if (0)
	{
	}
	// If we have a palette file, we must convert the GAL and save a palette'd BPD.
	{
	}

	// All the other layers may be saved in the same way as a JPEG. Only diffuse is ever palette'd.
	for (auto const& idp : read_data)
	{
		if (idp.usage != kTrImageDataUsagePalette)
		{
			// Save the data normally:
			arstring256 l_ouputFilename = l_outputFileStem + idp.outputSuffix + "." + idp.outputExt;
		}
	}

	return true;
}