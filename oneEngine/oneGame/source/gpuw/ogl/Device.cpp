﻿#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "./Device.h"
#include "./GraphicsContext.h"
#include "./ComputeContext.h"
#include "./OutputSurface.h"
#include "gpuw/Public/Error.h"
#include "renderer/types/types.h"
#include "./ogl/GLCommon.h"
#include "core/os.h"
#include "core/debug.h"

#include <stdio.h>

void APIENTRY DebugCallback(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​);

static gpu::Device* m_TargetDisplayDevice = NULL;

gpu::Device* gpu::getDevice ( void )
{
	return m_TargetDisplayDevice;
}

gpu::Device::Device ( intptr_t module_handle, intptr_t module_window )
	: mw_module(module_handle), mw_window(module_window),
	m_graphicsContext(NULL), m_computeContext(NULL),
	m_layers(NULL), m_layerCount(0)
{}
gpu::Device::~Device ( void )
{
	delete[] m_layers;

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

	delete m_graphicsContext;
	delete m_computeContext;
}

int gpu::Device::create ( DeviceLayer* layers, uint32_t layerCount )
{
	// Save layers for creation later
	if (layers != NULL && layerCount != 0)
	{
		m_layers = new DeviceLayer[layerCount];
		m_layerCount = layerCount;

		memcpy(m_layers, layers, sizeof(DeviceLayer) * m_layerCount);
	}

	// Create device context:
	mw_deviceContext = (intptr_t)GetDC((HWND)mw_window);
	if (mw_deviceContext == NIL)
	{
		printf("Could not create a device context.\n");
		return gpu::kErrorInvalidDevice;
	}

	return gpu::kError_SUCCESS;
}

int gpu::Device::initialize ( OutputSurface* surface )
{
	// Create temporary OpenGL context:
	HGLRC tempContext = wglCreateContext((HDC)mw_deviceContext);
	if (tempContext == NULL)
	{
		printf("Could not create an OpenGL context: Error code %x.\n", GetLastError());
		return gpu::kErrorInvalidDevice;
	}
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

	// Set target attributes to load OpenGL 4.7
	int target_major = 4;
	int target_minor = 7;

	// Load up the correct version of OpenGL
	mw_renderContext = NIL;
	do
	{
		// Attribute array
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, target_major,
			WGL_CONTEXT_MINOR_VERSION_ARB, target_minor,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

		// Set up attribs based on input options
		attribs[5] = 0;
		for (uint32_t i = 0; i < m_layerCount; ++i)
		{
			if (m_layers[i] == gpu::kDeviceLayerDebug)
			{	// Enable debug context if a debug "layer" is requested.
				attribs[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
			}
		}

		// Create the new context
		mw_renderContext = (intptr_t)wglCreateContextAttribsARB((HDC)mw_deviceContext, 0, attribs);	

		// Downgrade version to 4.5 only.
		if (mw_renderContext == NIL)
		{
			target_minor -= 1;
			if (target_minor < 5) {
				return gpu::kErrorModuleUnsupported;
			}
		}
	}
	while (mw_renderContext == NIL);

	// Delete the temporary context
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);

	// Set up the render context
	if (wglMakeCurrent((HDC)mw_deviceContext, (HGLRC)mw_renderContext) == FALSE)
	{
		printf("Could not make new context current. Error code %x.\n", GetLastError());
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

	// Update the target device:
	m_TargetDisplayDevice = this;

	// Enable debug "layer"
	for (uint32_t i = 0; i < m_layerCount; ++i)
	{
		if (m_layers[i] == gpu::kDeviceLayerDebug)
		{	
			glDebugMessageCallback(DebugCallback, this);
		}
	}

	// Update the Presentation settings
	switch (surface->m_presentMode)
	{
	case gpu::kPresentModeImmediate:
		wglSwapIntervalEXT(0);
		break;
	case gpu::kPresentModeFIFO:
		wglSwapIntervalEXT(1);
		break;
	case gpu::kPresentModeFIFORelaxed:
		wglSwapIntervalEXT(-1);
		break;
	case gpu::kPresentModeMailbox:
		wglSwapIntervalEXT(2);
		break;

	}

	return gpu::kError_SUCCESS;
}

int gpu::Device::refresh ( intptr_t module_handle, intptr_t module_window )
{
	// Destroy the device context
	if (mw_deviceContext != NIL)
	{
		if (ReleaseDC((HWND)mw_window, (HDC)mw_deviceContext) != 1)
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
	if (wglMakeCurrent((HDC)mw_deviceContext, (HGLRC)mw_renderContext) != TRUE)
	{
		printf("Could not make new context current.\n");
		return gpu::kErrorInvalidModule;
	}

	return gpu::kError_SUCCESS;
}

gpu::GraphicsContext* gpu::Device::getContext ( void )
{
	if (m_graphicsContext == NULL) {
		m_graphicsContext = new gpu::GraphicsContext();
	}
	return m_graphicsContext;
}

gpu::ComputeContext* gpu::Device::getComputeContext ( void )
{
	if (m_computeContext == NULL) {
		m_computeContext = new gpu::ComputeContext();
	}
	return m_computeContext;
}

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// TODO: proper message reporting
	ARCORE_ASSERT(type != GL_DEBUG_TYPE_ERROR);
	if (type == GL_DEBUG_TYPE_ERROR)
	{
	}

	// TODO
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
	{
	//	AR_DEBUG_BREAK();
	}

	// Copy-pasted from OpenGL wiki:
	/*fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);*/
}

#endif