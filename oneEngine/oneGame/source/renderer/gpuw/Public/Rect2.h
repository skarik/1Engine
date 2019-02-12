#ifndef GPU_WRAPPER_PUBLIC_RECT2_H_
#define GPU_WRAPPER_PUBLIC_RECT2_H_

#include <stdint.h>

namespace gpu
{
	struct Rect2
	{
		int32_t left;
		int32_t top;
		int32_t width;
		int32_t height;

		Rect2( int32_t left, int32_t top, int32_t width, int32_t height )
			: left(left), top(top), width(width), height(height)
			{}

		Rect2( void )
			: left(0), top(0), width(0), height(0)
			{}
	};
}

#endif//GPU_WRAPPER_PUBLIC_RECT2_H_