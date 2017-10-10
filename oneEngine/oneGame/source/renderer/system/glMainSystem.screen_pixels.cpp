
// Include the main system
#include "glMainSystem.h"
// Include screen properties
#include "core/system/Screen.h"

// Include the openGl window stuff to grab the handle
//#include "RrWindow.h"

// Clears the buffer. Kind of a repeated thingy, so rather not mess with it
void glMainSystem::ClearBuffer ( unsigned int bitmask )
{
	glClear(bitmask);	// Clear Screen And Depth Buffer
}

// Update the buffer with everythin that needs to be drawn
void glMainSystem::UpdateBuffer ( void )
{
	glFinish();
	//SwapBuffers( RrWindow::pActive->getDevicePointer() );
}

// Get a pixel from the screen
Color glMainSystem::GetPixel ( int x, int y )
{
	Color result;
	glReadPixels( x,y, 1,1, GL_RGBA, GL_FLOAT, result.raw );
	return result;
}

// Get a bunch of pixels from the screen (RGB)
float* glMainSystem::GetPixels( int x, int y, int w, int h )
{
	float* pVals = new float [w*h*3];
	glReadPixels( x,Screen::Info.height-y-h, w,h, GL_RGB, GL_FLOAT, pVals );
	return pVals;
}