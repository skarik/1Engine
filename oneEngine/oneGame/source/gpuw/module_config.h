#ifndef GPU_WRAPPER_MODULE_CONFIG_H_
#define GPU_WRAPPER_MODULE_CONFIG_H_

// OpenGL 4.6
// Due to shoddy driver support of OpenGL, no longer supported on Windows.
// Most features will be tested on OpenGL first. Use for base level support.
//#ifndef GPU_API_OPENGL
//#define GPU_API_OPENGL 1
//#endif

// Vulkan 1.0
// Not yet supported.
//#ifndef GPU_API_VULKAN
//#define GPU_API_VULKAN 1
//#endif

// DirectX 11.1
// Fully supported. Use on systems that do not support at least OpenGL 4.5.
#ifndef GPU_API_DIRECTX11
#define GPU_API_DIRECTX11 1
#endif

// DirectX 12.0
// Not yet supported.
//#ifndef GPU_API_DIRECTX12
//#define GPU_API_DIRECTX12 1
//#endif

// When enabled, inserts another layer of buffer for mapping. During unmap, checks if the edges of the buffer have been overwritten.
#ifndef GPU_API_DEBUG_MAP_OVERRUNS
#define GPU_API_DEBUG_MAP_OVERRUNS 0
#endif

#endif//GPU_WRAPPER_MODULE_CONFIG_H_