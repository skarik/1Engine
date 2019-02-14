#ifdef GPU_API_OPENGL

#include "./OutputSurface.h"
#include "./RenderTarget.h"
#include "./Device.h"
#include "gpuw/Public/Error.h"
#include "./ogl/GLCommon.h"
#include "renderer/types/types.h"
#include "core/os.h"
#include <stdio.h>

int gpu::OutputSurface::create ( Device* device, PresentMode presentMode, uint32_t width, uint32_t height )
{
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 32; // todo
	pfd.cDepthBits	= 0;
	pfd.cStencilBits= 0;
	pfd.iLayerType  = PFD_MAIN_PLANE;

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

	// Save the "requested" width and height.
	m_width = width;
	m_height = height;

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

gpu::RenderTarget* gpu::OutputSurface::getRenderTarget ( void )
{
	static RenderTarget rt;
	rt.m_framebuffer = 0xFFFFFFFF;
	rt.m_assembled = true;
	return &rt;
}

uint32_t gpu::OutputSurface::getWidth ( void )
{
	return m_width;
}
uint32_t gpu::OutputSurface::getHeight ( void )
{
	return m_height;
}

#endif