#ifndef TOOL_MODULE_CONVERT_32BIT_GRAPHICS_GALE_H_
#define TOOL_MODULE_CONVERT_32BIT_GRAPHICS_GALE_H_

#include "tool/converter/TrConverterBase.h"

#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"
#include "core/gfx/textureFormats.h"

class TrConverter32GraphicsGale : public TrConverterBase
{
public:
	bool Convert(const char* inputFilename, const char* outputFilename) override;
};

#endif//TOOL_MODULE_CONVERT_32BIT_GRAPHICS_GALE_H_