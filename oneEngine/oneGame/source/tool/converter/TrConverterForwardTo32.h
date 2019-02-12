#ifndef TOOL_MODULE_CONVERT_FORWARD_TO_32_H_
#define TOOL_MODULE_CONVERT_FORWARD_TO_32_H_

#include "tool/converter/TrConverterBase.h"

class TrConverterForwardTo32 : public TrConverterBase
{
public:
	bool Convert(const char* inputFilename, const char* outputFilename) override;
};

#endif//TOOL_MODULE_CONVERT_FORWARD_TO_32_H_