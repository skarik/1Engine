#include "renderer/gpuw/Fence.h"
#include "renderer/gpuw/Error.h"
#include "renderer/gpuw/ogl/GLCommon.h"

#include "core/debug.h"

int gpu::Fence::create ( Device* device )
{
	// Fences are created by the context as they're used, so we do nothing here except set default values:
	m_syncId = NULL;
	m_gfxc = NULL;
	m_compute = NULL;

	return kError_SUCCESS;
}

int gpu::Fence::destroy ( Device* device )
{
	return kError_SUCCESS;
}

int gpu::Fence::init ( void )
{
	if (m_syncId != NULL)
	{
		glDeleteSync((GLsync)m_syncId);
		m_syncId = NULL;
	}
	m_gfxc = NULL;
	m_compute = NULL;
	return kError_SUCCESS;
}

bool gpu::Fence::signaled ( void )
{
	// If the sync has been deleted, then it's been signaled.
	// However, this will also be true even if the sync is invalid or not used yet.
	// This shouldn't be a problem, as signaled should only be called after the fence is used as a sync.
	if (m_syncId == NULL)
	{
		return true;
	}

	ARCORE_ASSERT(m_gfxc != NULL || m_compute != NULL);

	// We want to check the fence with 0 timeout so we don't block the CPU.
	GLenum waitResult = glClientWaitSync((GLsync)m_syncId, 0, 0);

	ARCORE_ASSERT(waitResult != GL_WAIT_FAILED);

	// Check if the sync has already been signaled
	if (waitResult == GL_ALREADY_SIGNALED
		|| waitResult == GL_CONDITION_SATISFIED)
	{
		// When it has, free it up.
		glDeleteSync((GLsync)m_syncId);
		m_syncId = NULL; // Finish up.
		return true;
	}
	return false;
}

bool gpu::Fence::valid ( void )
{
	// Fences are created by the context as they're added to the stream, so we always return true:
	return true;
}

gpuHandle gpu::Fence::nativePtr ( void )
{
	return (gpuHandle)m_syncId;
}