#ifndef RENDER2D_PREPROCESS_CONVERSION_H_
#define RENDER2D_PREPROCESS_CONVERSION_H_

#include "core/types.h"

namespace render2d
{
	namespace preprocess
	{
		struct rrConvertSpriteResults
		{
			bool			hasAlbedo;
			bool			hasNormals;
			bool			hasSurface;
			bool			hasIllumin;
			bool			hasAnimation;
		};

		//	ConvertSprite(resource_name) : Finds the given sprite and attempts to convert it to BPD resources.
		// Will return true if a color sprite (albedo) exists.
		RENDER2D_API bool		ConvertSprite ( const char* resource_name, rrConvertSpriteResults* convert_results );
	}
}

#endif//RENDER2D_PREPROCESS_CONVERSION_H_