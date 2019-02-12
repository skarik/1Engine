#ifndef TOOL_MODULE_CONVERT_PNG_H_
#define TOOL_MODULE_CONVERT_PNG_H_

#include "tool/converter/TrConverterBase.h"

class TrConverterPNG : public TrConverterBase
{
public:
	bool Convert(const char* inputFilename, const char* outputFilename) override;
};

#endif//TOOL_MODULE_CONVERT_PNG_H_