#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./Device.dx11.h"
#include "./GraphicsContext.dx11.h"
#include "./ComputeContext.dx11.h"
#include "./OutputSurface.dx11.h"
#include "gpuw/Public/Error.h"
#include "renderer/types/types.h"
#include "core/os.h"
#include "core/debug.h"

#include <vector>
#include <stdio.h>

static gpu::Device* m_TargetDisplayDevice = NULL;

gpu::Device* gpu::getDevice ( void )
{
	return m_TargetDisplayDevice;
}

gpu::Device::Device ( intptr_t module_handle, intptr_t module_window )
	: mw_module(module_handle), mw_window(module_window),
	m_graphicsContext(NULL), m_computeContext(NULL)
	//m_layers(NULL), m_layerCount(0)
{}
gpu::Device::~Device ( void )
{
	/*delete[] m_layers;

	// Destroy the device context
	if (mw_deviceContext != NIL)
	{
		if (ReleaseDC((HWND)mw_window, (HDC)mw_deviceContext) != 0)
		{
			printf("Release Device Context Failed.\n");
			//core::shell::ShowErrorMessage("Release Device Context Failed.");
		}
		mw_deviceContext = NIL;
	}*/

	delete m_graphicsContext;
	delete m_computeContext;
}

int gpu::Device::create ( DeviceLayer* layers, uint32_t layerCount )
{
	//IDXGIFactory	*dx_factory;
	std::vector<IDXGIAdapter*>
					dx_adapters;
	//IDXGIAdapter*	dx_selectedAdapter;
	//std::vector<IDXGIOutput*>
	//				dx_adapterOutputs;
	HRESULT			result;
	
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_dxFactory);
	if (FAILED(result))
	{
		printf("Could not create IDXGIFactory.\n");
		return gpu::kErrorCreationFailed;
	}

	// TODO: Make device selection an externally configurable option.

	// Use factory to grab all graphics adapters
	UINT			i_currentAdapterIndex = 0;
	IDXGIAdapter	*l_currentAdapter = NULL;
	while (m_dxFactory->EnumAdapters(i_currentAdapterIndex, &l_currentAdapter) != DXGI_ERROR_NOT_FOUND) 
	{
		dx_adapters.push_back(l_currentAdapter);
		++i_currentAdapterIndex;
	}
	// Select the first graphics adapter in the list:
	m_dxAdapter = dx_adapters[0];
	// Grab the information for debug info
	DXGI_ADAPTER_DESC l_adapterDescription;
	m_dxAdapter->GetDesc(&l_adapterDescription);

	printf("Creating DirectX device on \"%S\"\n", l_adapterDescription.Description);

	// Update the target device:
	m_TargetDisplayDevice = this;

	// Grab all the output adapters for the graphics adapater
	/*i_currentAdapterIndex = 0;
	IDXGIOutput		*l_currentOutputAdapter = NULL;
	while (m_dxAdapter->EnumOutputs(i_currentAdapterIndex, &l_currentOutputAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		dx_adapterOutputs.push_back(l_currentOutputAdapter);
		++i_currentAdapterIndex;
	}*/
	// TODO: pull from layers arg
	UINT layerFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE;
	D3D_FEATURE_LEVEL featureLevels [] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
	D3D_FEATURE_LEVEL actualFeatureLevel;

	do
	{
		//result = D3D11CreateDevice(	NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		result = D3D11CreateDevice(	m_dxAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
									layerFlags,
									featureLevels, sizeof(featureLevels)/sizeof(D3D_FEATURE_LEVEL),
									D3D11_SDK_VERSION,
									&m_dxDevice, &actualFeatureLevel,
									&m_dxImmediateContext);

		if ((result == DXGI_ERROR_SDK_COMPONENT_MISSING || result == DXGI_ERROR_UNSUPPORTED)
			&& layerFlags == (D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE))
		{
			printf("Could not find DirectX debug layer. Reducing options.\n");
			layerFlags = D3D11_CREATE_DEVICE_DEBUG;
			continue;
		}
		else if ((result == DXGI_ERROR_SDK_COMPONENT_MISSING || result == DXGI_ERROR_UNSUPPORTED)
			&& layerFlags == D3D11_CREATE_DEVICE_DEBUG)
		{
			printf("Could not find DirectX debug layer. Please install D3D11*SDKLayers.dll.\n");
			layerFlags = 0;
			continue;
		}
		else if (result != S_OK)
			break;
	}
	while (result != S_OK);

	if (FAILED(result))
	{
		printf("Could not create device or device context.\n");
		return gpu::kErrorInvalidDevice;
	}

#ifdef _ENGINE_DEBUG
	// Disable specific error keys
	ID3D11Debug* l_dxDebug;
	result = m_dxDevice->QueryInterface(&l_dxDebug);
	if (SUCCEEDED(result))
	{
		ID3D11InfoQueue* l_dxInfoQueue;
		result = l_dxDebug->QueryInterface(&l_dxInfoQueue);
		if (SUCCEEDED(result))
		{
			D3D11_MESSAGE_ID toHide [] = {
				D3D11_MESSAGE_ID_DEVICE_DRAW_VERTEX_BUFFER_STRIDE_TOO_SMALL, // Enable use of purposefully overlapping data
			};

			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = sizeof(toHide) / sizeof(D3D11_MESSAGE_ID);
			filter.DenyList.pIDList = toHide;

			l_dxInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	return gpu::kError_SUCCESS;
}

int gpu::Device::initialize ( OutputSurface* surface )
{
	// Nothing on DX11.

	return gpu::kError_SUCCESS;
}

int gpu::Device::refresh ( intptr_t module_handle, intptr_t module_window )
{
	// Update context
	mw_module = module_handle;
	mw_window = module_window;

	// The output surface now has to be recreated, but is done by the caller.

	return gpu::kError_SUCCESS;
}

gpu::GraphicsContext* gpu::Device::getContext ( void )
{
	if (m_graphicsContext == NULL) {
		m_graphicsContext = new gpu::GraphicsContext(this);
	}
	return m_graphicsContext;
}

gpu::ComputeContext* gpu::Device::getComputeContext ( void )
{
	if (m_computeContext == NULL) {
		m_computeContext = new gpu::ComputeContext(this);
	}
	return m_computeContext;
}

// Grab native device object
ID3D11Device* gpu::Device::getNative ( void )
{
	return m_dxDevice;
}

ID3D11DeviceContext* gpu::Device::getNativeContext ( void )
{
	return m_dxImmediateContext;
}

/*void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	ARCORE_ASSERT(type != GL_DEBUG_TYPE_ERROR);
	if (type == GL_DEBUG_TYPE_ERROR)
	{
	}
}*/

#endif