
#ifndef _ENGINE_SCREEN_H_
#define _ENGINE_SCREEN_H_

#include "core/types/types.h"

namespace Screen
{
	struct _screen_info_t
	{
	public:
		CORE_API void Update ( void );

	public:
		unsigned int width;
		unsigned int height;
		double aspect;

		float scale;
	};
	CORE_API extern _screen_info_t Info;
};


#endif//_ENGINE_SCREEN_H_