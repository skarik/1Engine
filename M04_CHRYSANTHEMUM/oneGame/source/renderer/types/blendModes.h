
#ifndef _RENDERER_BLEND_MODES_H_
#define _RENDERER_BLEND_MODES_H_

#include "core/types/types.h"

enum eBlendModes
{
	BM_ZERO,
	BM_ONE,
	BM_SRC_COLOR,
	BM_SRC_ALPHA,
	BM_DEST_COLOR,
	BM_DEST_ALPHA,

	BM_INV_SRC_COLOR,
	BM_INV_SRC_ALPHA,
	BM_INV_DEST_COLOR,
	BM_INV_DEST_ALPHA
};

#endif//_RENDERER_BLEND_MODES_H_