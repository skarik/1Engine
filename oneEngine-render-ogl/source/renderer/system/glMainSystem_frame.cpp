
#include "glMainSystem.h"
#include "glDrawing.h"

#include "renderer/window/COglWindow.h"
#include "renderer/state/CRenderState.h"

#include <thread>
#include <atomic>

std::thread*		presentThread = NULL;
std::atomic_flag	presentLock = ATOMIC_FLAG_INIT;

void performPresent ( void )
{
#ifdef _WIN32
	SwapBuffers(COglWindow::pActive->getDevicePointer());				// Swap Buffers (Double Buffering) (VSYNC)
#endif

	presentLock.clear();
}

// Present the current frame
void glMainSystem::Present ( void )
{
	/*while ( presentLock.test_and_set() ) {
		std::this_thread::yield();
	}*/
	//presentThread = new std::thread( performPresent );
}
// Prepare system for a new frame
void glMainSystem::BeginFrame ( void )
{
	/*if ( presentThread ) {
		presentThread->join();
		delete presentThread;
	}*/
}
// Cleanup after old frame
void glMainSystem::EndFrame ( void )
{	// Using the glMainSystem accessor
	GLd_ACCESS;

	// Swap Buffers (Double Buffering)
	SwapBuffers(COglWindow::pActive->getDevicePointer());

	// Mark that we're done with rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Mark buffers as free to use
	GLd.MarkBuffersFreeUsage();
}


// Get the current main screen buffer
CRenderTexture* glMainSystem::GetMainScreenBuffer ( void )
{
	//return COglWindow::pActive->getScreenBuffer();
	return SceneRenderer->GetForwardBuffer();
}

// Tell the entire system to restart
void glMainSystem::FullRedraw( void )
{
	COglWindow::pActive->Redraw();
}