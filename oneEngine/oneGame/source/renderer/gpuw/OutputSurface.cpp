#include "renderer/gpuw/OutputSurface.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/Error.h"
#include "renderer/ogl/GLCommon.h"
#include "renderer/types/types.h"
#include "core/os.h"
#include <stdio.h>

int gpu::OutputSurface::create ( Device* device, PresentMode presentMode )
{
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 32; // todo
	pfd.cDepthBits	= 0;
	pfd.cStencilBits= 0;

	// Did Windows Find A Matching Pixel Format?
	int PixelFormat = ChoosePixelFormat((HDC)device->mw_deviceContext, &pfd);
	if (PixelFormat == 0)
	{
		printf("Can't Find A Suitable PixelFormat.\n");
		return gpu::kErrorFormatUnsupported;
	}
	// Are We Able To Set The Pixel Format?
	if(SetPixelFormat((HDC)device->mw_deviceContext, PixelFormat, &pfd) == 0)
	{
		printf("Can't Set The PixelFormat.\n");
		return gpu::kErrorFormatUnsupported;
	}

	// TODO: VSync settings with presentMode.

	// Save the context:
	mw_deviceContext = device->mw_deviceContext;

	return 0;
}

int gpu::OutputSurface::destroy ( void )
{
	return 0;
}

int gpu::OutputSurface::present ( void )
{
	BOOL result = SwapBuffers((HDC)mw_deviceContext);
	return (result == TRUE);
}