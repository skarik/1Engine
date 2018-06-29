#ifndef TOOL_MODULE_CONVERTER_BASE_H_
#define TOOL_MODULE_CONVERTER_BASE_H_

class TrConverterBase
{
public:

	virtual bool Convert(const char* inputFilename, const char* outputFilename)
		{ return false; }
};

#endif//TOOL_MODULE_CONVERTER_BASE_H_