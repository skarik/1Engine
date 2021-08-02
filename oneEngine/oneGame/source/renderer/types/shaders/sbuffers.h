#ifndef RENDERER_SHADER_STRUCTURED_BUFFER_TYPES_H_
#define RENDERER_SHADER_STRUCTURED_BUFFER_TYPES_H_

#include "core/math/Math3d.h"
#include <stdint.h>

namespace renderer
{
	// Since Cbuffers and Sbuffers share binding targets on the backend, we start at 11.
	enum rrSBufferId
	{
		SBUFFER_SKINNING_MAJOR = 11,
		SBUFFER_SKINNING_MINOR = 12,

		SBUFFER_USER0 = 13,
		SBUFFER_USER1 = 14,
		SBUFFER_USER2 = 15,
		SBUFFER_USER3 = 16,
	};

	namespace sbuffer
	{
	}
}

#endif RENDERER_SHADER_STRUCTURED_BUFFER_TYPES_H_