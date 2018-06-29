#include "core/common.h"
#include "core/utils/string.h"
#include "core-ext/system/io/assets/Conversion.h"

#include <vector>
#include <utility>

#include "tool/converter/TrConverterBase.h"
#include "tool/converter32/TrConverter32GraphicsGale.h"

int main ( int argc, char **argv )
{	
	if (argc < 3)
	{
		printf("Missing arguments.\n");
		printf("Usage: oneConverter <input> <output>\n");
		printf("Conversion used is based on input and output extensions.\n");
		return 1;
	}

	typedef TrConverterBase* (*trConverterInstantiator)(void);
	typedef std::tuple<arstring64, arstring64, trConverterInstantiator> trConversionEntry;
	const char* kEngineModelFormat = "pad";
	const char* kEngineImageFormat = "bpd";

	std::vector<trConversionEntry> l_conversionTable;
	l_conversionTable.push_back(trConversionEntry("gal", kEngineImageFormat, (trConverterInstantiator)_instantiate<TrConverter32GraphicsGale, TrConverterBase>));

	// Parse the arguments:
	std::string l_inputFilename(argv[1]);
	std::string l_outputFilename(argv[2]);

	std::string l_inputExtension = core::utils::string::GetLower(core::utils::string::GetFileExtension(l_inputFilename));
	std::string l_outputExtension = core::utils::string::GetLower(core::utils::string::GetFileExtension(l_outputFilename));

	// Find matching entry in the table
	for (const trConversionEntry& entry : l_conversionTable)
	{
		const arstring64& inputExtension  = std::get<0>(entry);
		const arstring64& outputExtension = std::get<1>(entry);
		trConverterInstantiator instantiator = std::get<2>(entry);

		if (instantiator != NULL)
		{
			if (inputExtension.compare(l_inputExtension.c_str()) && outputExtension.compare(l_outputExtension.c_str()))
			{
				// Run with the new test type.
				TrConverterBase* converter = instantiator();
				converter->Convert(l_inputFilename.c_str(), l_outputFilename.c_str());
				delete converter;
				return 0;
			}
		}
	}

	printf("No suitable converter was found.\n");
	return 1;
}