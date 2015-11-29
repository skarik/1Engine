
#include "glMainSystem.h"
#include "glDrawing.h"

#include "renderer/window/COglWindow.h"

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
	SwapBuffers(COglWindow::pActive->getDevicePointer());				// Swap Buffers (Double Buffering) (VSYNC)
	/*if ( presentThread ) {
		presentThread->join();
		delete presentThread;
	}*/
}
// Cleanup after old frame
void glMainSystem::EndFrame ( void )
{
	GLd_ACCESS; // Using the glMainSystem accessor
	// Mark buffers as free to use
	GLd.MarkBuffersFreeUsage();
}


// Get the current main screen buffer
CRenderTexture* glMainSystem::GetMainScreenBuffer ( void )
{
	return COglWindow::pActive->getScreenBuffer();
}

// Tell the entire system to restart
void glMainSystem::FullRedraw( void )
{
	COglWindow::pActive->Redraw();
}