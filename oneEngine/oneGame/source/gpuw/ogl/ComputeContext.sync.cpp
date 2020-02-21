#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "core/types/types.h"
#include "core/exceptions.h"

#include "gpuw/Public/Error.h"

#include "./ComputeContext.h"
#include "./Buffers.h"
#include "./Pipeline.h"
#include "./ShaderPipeline.h"
#include "./Fence.h"
#include "./ogl/GLCommon.h"

#include "renderer/types/types.h"

int gpu::ComputeContext::signal ( Fence* fence )
{
	ARCORE_ASSERT(fence->m_syncId == NULL);
	ARCORE_ASSERT(fence->m_compute == NULL);

	// Create the sync the only way OpenGL allows
	fence->m_syncId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Tag this context in the fence
	fence->m_compute = this;

	// If the fence was created, flush the buffer now:
	if (fence->m_syncId != NULL) {
		glFlush();
	}

	// Return that it's not a nullptr.
	return (fence->m_syncId != NULL) ? kError_SUCCESS : kErrorNullReference;
}
int gpu::ComputeContext::waitOnSignal ( Fence* fence )
{
	ARCORE_ASSERT(fence->m_syncId != NULL);
	ARCORE_ASSERT(fence->m_gfxc != NULL || fence->m_compute != NULL);

	// The sync is already created, so we just wait on it:
	glWaitSync((GLsync)fence->m_syncId, 0, GL_TIMEOUT_IGNORED);

	// Tag this context in the fence
	fence->m_compute = this;

	return kError_SUCCESS;
}

#endif