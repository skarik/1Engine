#ifndef GPU_WRAPPER_DX11_GPU_H_
#define GPU_WRAPPER_DX11_GPU_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#if _ENGINE_DEBUG
#pragma comment(lib, "dxguid.lib")
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11_2.h>

#endif//GPU_WRAPPER_DX11_GPU_H_