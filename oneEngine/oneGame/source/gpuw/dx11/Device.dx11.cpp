﻿#include "gpuw/gpuw_common.h"
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

/*static gpu::Device* m_TargetDisplayDevice = NULL;

gpu::Device* gpu::getDevice ( void )
{
	return m_TargetDisplayDevice;
}*/

gpu::dx11::Device::Device ( void )
	//: mw_module(module_handle), mw_window(module_window),
	//m_graphicsContext(NULL), m_computeContext(NULL)
	//m_layers(NULL), m_layerCount(0)
{}
gpu::dx11::Device::~Device ( void )
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

	//delete m_graphicsContext;
	//delete m_computeContext;
	free();
}

int gpu::dx11::Device::create ( DeviceFeature* features, uint32_t featureCount, DeviceLayer* layers, uint32_t layerCount )
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
		//result = D3D11CreateDevice(	m_dxAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
		result = D3D11CreateDevice(	NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
									layerFlags,
									featureLevels, sizeof(featureLevels)/sizeof(D3D_FEATURE_LEVEL),
									D3D11_SDK_VERSION,
									&m_dxDevice, &actualFeatureLevel,
									&m_dxImmediateContext);

		if ((result == DXGI_ERROR_SDK_COMPONENT_MISSING || result == DXGI_ERROR_UNSUPPORTED)
			&& layerFlags == (D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE))
		{
			printf("Could not find DirectX debug layer (D3D11_CREATE_DEVICE_DEBUGGABLE). Reducing options.\n");
			layerFlags = D3D11_CREATE_DEVICE_DEBUG;
			continue;
		}
		else if ((result == DXGI_ERROR_SDK_COMPONENT_MISSING || result == DXGI_ERROR_UNSUPPORTED)
			&& layerFlags == D3D11_CREATE_DEVICE_DEBUG)
		{
			printf("Could not find DirectX debug layer (D3D11_CREATE_DEVICE_DEBUG). Please install D3D11*SDKLayers.dll.\n");
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

			result = l_dxInfoQueue->AddStorageFilterEntries(&filter);
			if (FAILED(result))
			{
				printf("Could not filter warning messages.\n");
			}
		}
	}
#endif

	return gpu::kError_SUCCESS;
}

int gpu::dx11::Device::initialize ( OutputSurface* surface )
{
	// Nothing on DX11.

	return gpu::kError_SUCCESS;
}

// Grab native device object
ID3D11Device* gpu::dx11::Device::getNative ( void )
{
	return m_dxDevice;
}

ID3D11DeviceContext* gpu::dx11::Device::getImmediateContext ( void )
{
	return m_dxImmediateContext;
}

int gpu::dx11::Device::free ( void )
{
	if (m_dxImmediateContext)
	{
		m_dxImmediateContext->Release();
		m_dxImmediateContext = NULL;
	}

	if (m_dxDevice)
	{
		m_dxDevice->Release();
		m_dxDevice = NULL;
	}

	if (m_dxAdapter)
	{
		m_dxAdapter->Release();
		m_dxAdapter = NULL;
	}

	if (m_dxFactory)
	{
		m_dxFactory->Release();
		m_dxFactory = NULL;
	}

	return gpu::kError_SUCCESS;
}

#endif