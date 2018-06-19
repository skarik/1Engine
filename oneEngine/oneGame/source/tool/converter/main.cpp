#include "core/debug/console.h"
#include "core/containers/arstring.h"

#include <vector>
#include <utility>

class TrConverterBase;

int main ( int argc, char **argv )
{	
	if (argc < 3)
	{
		printf("Missing arguments.\n");
		printf("Usage: oneConverter <input> <output>\n");
		printf("Conversion used is based on input and output extensions.\n");
		return 1;
	}

	typedef std::tuple<arstring64, arstring64, TrConverterBase*> trConversionEntry;
	const char* kEngineModelFormat = "pad";
	const char* kEngineImageFormat = "bpd";

	std::vector<trConversionEntry> l_conversionTable;
	l_conversionTable.push_back(trConversionEntry("fbx", kEngineModelFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("gltf", kEngineModelFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("gal", kEngineImageFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("png", kEngineImageFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("jpg", kEngineImageFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("jpeg", kEngineImageFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("tga", kEngineImageFormat, NULL));
	l_conversionTable.push_back(trConversionEntry("gif", kEngineImageFormat, NULL));

	return 0;
}