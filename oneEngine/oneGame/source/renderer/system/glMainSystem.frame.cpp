#include "glMainSystem.h"
#include "glDrawing.h"

#include "renderer/window/RrWindow.h"
#include "renderer/state/CRenderState.h"

#include <thread>
#include <atomic>

std::thread*		presentThread = NULL;
std::atomic_flag	presentLock = ATOMIC_FLAG_INIT;

void performPresent ( void )
{
#ifdef _WIN32
	// Swap Buffers (Double Buffering) (VSYNC)
	SwapBuffers(RrWindow::pActive->getDevicePointer());
#endif

	presentLock.clear();
}

// Prepare system for a new frame
void glMainSystem::BeginFrame ( void )
{
	;
}
// Cleanup after old frame
void glMainSystem::EndFrame ( void )
{	// Using the glMainSystem accessor
	GLd_ACCESS;

	// Swap Buffers (Double Buffering)
	SwapBuffers(RrWindow::pActive->getDevicePointer());

	// Mark that we're done with rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


// Get the current main screen buffer
CRenderTexture* glMainSystem::GetMainScreenBuffer ( void )
{
	//return RrWindow::pActive->getScreenBuffer();
	return SceneRenderer->GetForwardBuffer();
}

// Tell the entire system to restart
void glMainSystem::FullRedraw( void )
{
	RrWindow::pActive->Redraw();
}