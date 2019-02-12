#include "GraphicsContext.h"
#include "Error.h"
#include "core/debug.h"
#include "renderer/gpuw/Fence.h"
#include "renderer/gpuw/Internal/Enums.h"
#include "renderer/ogl/GLCommon.h"
#include <stdio.h>

int gpu::GraphicsContext::signal ( Fence* fence )
{
	//ARCORE_ERROR("not implemented");

	ARCORE_ASSERT(fence->m_syncId == NULL);
	ARCORE_ASSERT(fence->m_gfxc == NULL);

	// Create the sync the only way OpenGL allows
	fence->m_syncId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	// Tag this context in the fence
	fence->m_gfxc = this;

	// If the fence was created, flush the buffer now:
	if (fence->m_syncId != NULL) {
		glFlush();
	}

	// Return that it's not a nullptr.
	return (fence->m_syncId != NULL) ? kError_SUCCESS : kErrorNullReference;
}
int gpu::GraphicsContext::waitOnSignal ( Fence* fence )
{
	ARCORE_ASSERT(fence->m_syncId != NULL);
	ARCORE_ASSERT(fence->m_gfxc != NULL || fence->m_compute != NULL);

	// The sync is already created, so we just wait on it:
	glWaitSync((GLsync)fence->m_syncId, 0, GL_TIMEOUT_IGNORED);

	// Tag this context in the fence
	fence->m_gfxc = this;

	return kError_SUCCESS;
}
