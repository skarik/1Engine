#ifndef ENGINE_COMMON_DUSK_UI_LAYOUT_COMMON_H_
#define ENGINE_COMMON_DUSK_UI_LAYOUT_COMMON_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace layouts {

	enum class AlignStyleHorizontal
	{
		kLeft,
		kCenter,
		kRight,
	};

	enum class AlignStyleVertical
	{
		kTop,
		kMiddle,
		kBottom,
	};

	enum class JustifyScaleStyle
	{
		// Items are only spaced by padding
		kInline,
		// Padding is ignored, and items are spaced equidistantly
		kSpaceBetween,
		// Padding is ignored, and items are stretched to fill the space.
		// Items will be scaled proportional to the total size of items.
		kStretch,
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_LAYOUT_COMMON_H_