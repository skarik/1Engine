#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/GraphicsContext.h"
#include "renderer/gpuw/ComputeContext.h"
#include "renderer/gpuw/Error.h"
#include "renderer/types/types.h"
#include "renderer/ogl/GLCommon.h"
#include "core/os.h"

#include <stdio.h>

gpu::Device::Device ( intptr_t module_handle, intptr_t module_window )
	: mw_module(module_handle), mw_window(module_window)
{}
gpu::Device::~Device ( void )
{
	// Destroy the device context
	if (mw_deviceContext != NIL)
	{
		if (ReleaseDC((HWND)mw_window, (HDC)mw_deviceContext) != 0)
		{
			printf("Release Device Context Failed.\n");
			//core::shell::ShowErrorMessage("Release Device Context Failed.");
		}
		mw_deviceContext = NIL;
	}
}

int gpu::Device::create ( void )
{
	// Create device context:
	mw_deviceContext = (intptr_t)GetDC((HWND)mw_window);
	if (mw_deviceContext == NIL)
	{
		printf("Could not create a device context.\n");
		return gpu::kErrorInvalidDevice;
	}

	// Create temporary OpenGL context:
	HGLRC tempContext = wglCreateContext((HDC)mw_deviceContext);
	wglMakeCurrent((HDC)mw_deviceContext, tempContext);

	// Attempt to load up Windows OGL extensions
	if (wglgLoadFunctions() < 0)
	{
		printf("Could not load opengl windows module.\n");
		wglCreateContextAttribsARB = NULL;
		return gpu::kErrorInvalidModule;
	}
	if (wglCreateContextAttribsARB == NULL)
	{
		printf("Could not load wglCreateContextAttribsARB. Needed for OpenGL 3.2 and up.\n");
		return gpu::kErrorModuleUnsupported;
	}

	// Set target attributes to load OpenGL 4.6
	int target_major = 4;
	int target_minor = 6;

	// Load up the correct version of OpenGL
	mw_renderContext = NIL;
	do
	{
		// Attribute array
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, target_major,
			WGL_CONTEXT_MINOR_VERSION_ARB, target_minor,
			//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

		// Create the new context
		mw_renderContext = (intptr_t)wglCreateContextAttribsARB((HDC)mw_deviceContext, 0, attribs);	

		// Downgrade version to 4.3 only.
		if (mw_renderContext == NIL)
		{
			target_minor -= 1;
			if (target_minor < 3) {
				return gpu::kErrorModuleUnsupported;
			}
		}
	}
	while (mw_renderContext == NIL);

	// Delete the temporary context
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);

	// Set up the render context
	if (wglMakeCurrent((HDC)mw_deviceContext, (HGLRC)mw_renderContext) != 0)
	{
		printf("Could not make new context current.\n");
		return gpu::kErrorInvalidModule;
	}

	// Load up the OpenGL function pointers:
	int failed_loads = glgLoadFunctions();
	if (failed_loads < 0)
	{
		printf("Could not load OpenGL functions.\n");
		return gpu::kErrorModuleUnsupported;
	}
	else
	{
		printf("OpenGL loaded with %d missing functions.\n", failed_loads);
	}
	// Load up the Windows OpenGL function pointers:
	failed_loads = wglgLoadFunctions();
	if (failed_loads < 0)
	{
		printf("Could not load OpenGL Windows extension functions.\n");
		return gpu::kErrorModuleUnsupported;
	}
	else
	{
		printf("OpenGL Windows extension loaded with %d missing functions.\n", failed_loads);
	}

	// Clear out errors now
	while ( glGetError() != 0 ) { ; }

	// Query version number:
	int OpenGLVersion[2] = {0, 0};
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	printf("Created an OpenGL %d.%d context\n", OpenGLVersion[0], OpenGLVersion[1]);

	// Query vendor and renderer:
	const uchar* vendor		= glGetString(GL_VENDOR);
	const uchar* renderer	= glGetString(GL_RENDERER);
	printf("device vendor: %s\n", vendor);
	printf("     renderer: %s\n", renderer);
}

int gpu::Device::refresh ( intptr_t module_handle, intptr_t module_window )
{
	// Destroy the device context
	if (mw_deviceContext != NIL)
	{
		if (ReleaseDC((HWND)mw_window, (HDC)mw_deviceContext) != 0)
		{
			printf("Release Device Context Failed.\n");
			return gpu::kErrorInvalidDevice;
		}
		mw_deviceContext = NIL;
	}

	// Update context
	mw_module = module_handle;
	mw_window = module_window;

	// Create new context and associate OpenGL with it
	mw_deviceContext = (intptr_t)GetDC((HWND)mw_window);
	if (mw_deviceContext == NIL)
	{
		printf("Could not create a device context.\n");
		return gpu::kErrorInvalidDevice;
	}

	// Set up the render context
	if (wglMakeCurrent((HDC)mw_deviceContext, (HGLRC)mw_renderContext) != 0)
	{
		printf("Could not make new context current.\n");
		return gpu::kErrorInvalidModule;
	}
}
