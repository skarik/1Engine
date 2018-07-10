#ifndef GPU_WRAPPER_INPUT_LAYOUT_H_
#define GPU_WRAPPER_INPUT_LAYOUT_H_

#include "core/types/types.h"
#include "renderer/types/types.h"

namespace gpu
{
	// Input layout for buffers to shaders. The chief bad boy of the class.
	class InputLayout
	{
	public:
		//	Constructor : creates uninitalized GPU wrapper object.
		RENDER_API explicit		InputLayout ( void );
		//	Destructor : destroys any allocated VAO/Layout, if existing.
		RENDER_API 				~InputLayout ( void );
	};
}

#endif//GPU_WRAPPER_INPUT_LAYOUT_H_