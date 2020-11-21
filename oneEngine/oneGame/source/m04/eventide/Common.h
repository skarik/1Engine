#ifndef UI_EVENTIDE_COMMON_H_
#define UI_EVENTIDE_COMMON_H_

#include "core/types/types.h"
#include "core/math/Math3d.h"
#include "core/math/BoundingBox.h"

#ifndef EVENTIDE_API 
#define EVENTIDE_API GAME_API
#endif

class RrTexture;

namespace ui {
namespace eventide {

	struct Texture
	{
		// Eventually index to something but for now, it's just another texture
		RrTexture*				reference = NULL;
		// We use this to pass into params so things get the right texture
		uint32_t				index;
	};

}}

#endif//UI_EVENTIDE_COMMON_H_