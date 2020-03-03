#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./OutputSurface.dx11.h"
#include "./RenderTarget.dx11.h"
#include "./Device.dx11.h"
#include "gpuw/Public/Error.h"
#include "./gpu.h"
#include "renderer/types/types.h"
#include "core/os.h"
#include <stdio.h>

int gpu::OutputSurface::create ( Device* device, PresentMode presentMode, uint32_t width, uint32_t height, OutputFormat format, bool fullscreen )
{
	//swapchain_desc
	HRESULT result;

	DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
	swapchain_desc.BufferDesc.Width = width;
	swapchain_desc.BufferDesc.Height = height;
	if (presentMode == kPresentModeImmediate)
	{
		swapchain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
	{
		swapchain_desc.BufferDesc.RefreshRate.Numerator = 0;// TODO: Grab the display adapter
		swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: HDR
	swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapchain_desc.SampleDesc.Count = 1; // Disable multisampling
	swapchain_desc.SampleDesc.Quality = 0;

	swapchain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.BufferCount = 3;

	swapchain_desc.OutputWindow = (HWND)device->mw_window;
	swapchain_desc.Windowed = fullscreen ? FALSE : TRUE;

	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // TODO: Flip discard, one day.
	swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	// Given the device, create a swapchain
	result = device->m_dxFactory->CreateSwapChain((ID3D11Device*)device->getNative(), &swapchain_desc, (IDXGISwapChain**)&m_dxSwapchain);
	if (FAILED(result))
	{
		printf("Could not create surface (error: %x).\n", (int)result);
		return gpu::kErrorFormatUnsupported;
	}

	printf("created win32_surface.\n");

	// save the "requested" width and height.
	m_width = width;
	m_height = height;

	// Create a render target from the swapchain now:
	ID3D11Texture2D* l_renderTexture;

	m_renderTarget = new RenderTarget();
	m_renderTarget->create(device);

	static_cast<IDXGISwapChain*>(m_dxSwapchain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&l_renderTexture);

	device->getNative()->CreateRenderTargetView(l_renderTexture, NULL, (ID3D11RenderTargetView**)&m_renderTarget->m_attachments[0]);

	m_renderTarget->assemble();

	return 0;
}

int gpu::OutputSurface::destroy ( void )
{
	//vkDestroySurfaceKHR(m_device->m_instance, (VkSurfaceKHR)m_surface, m_device->m_allocator);
	if (m_dxSwapchain)
		static_cast<IDXGISwapChain*>(m_dxSwapchain)->Release();
	m_dxSwapchain = NULL;

	if (m_renderTarget)
		delete m_renderTarget;
	m_renderTarget = NULL;

	return 0;
}

int gpu::OutputSurface::present ( void )
{
	HRESULT result;
	result = static_cast<IDXGISwapChain*>(m_dxSwapchain)->Present(0, 0); // TODO.
	return (result == S_OK) ? kError_SUCCESS : result;
}

gpu::RenderTarget* gpu::OutputSurface::getRenderTarget ( void )
{
	return m_renderTarget;
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