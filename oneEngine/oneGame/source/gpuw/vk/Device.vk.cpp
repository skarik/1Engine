#include "gpuw/gpuw_common.h"
#ifdef GPU_API_VULKAN

#include "./Device.vk.h"
#include "./GraphicsContext.vk.h"
#include "./ComputeContext.vk.h"
#include "./OutputSurface.vk.h"
#include "gpuw/Public/Error.h"
#include "renderer/types/types.h"
#include "./gpu.h"
#include "core/common.h"
#include "core/os.h"
#include "core/debug.h"

#include <vector>
#include <stdio.h>

//typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
//void APIENTRY DebugCallback(GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​);
static VKAPI_ATTR VkBool32 VKAPI_CALL
rvkDebugReportCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
	int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData);

static gpu::Device* m_TargetDisplayDevice = NULL;

gpu::Device* gpu::getDevice ( void )
{
	return m_TargetDisplayDevice;
}

gpu::Device::Device ( intptr_t module_handle, intptr_t module_window )
	: mw_module(module_handle), mw_window(module_window),
	m_allocator(NULL),
	m_graphicsContext(NULL), m_computeContext(NULL)
{}
gpu::Device::~Device ( void )
{
	// Destroy the device context
	//if (mw_deviceContext != NIL)
	{
		/*if (ReleaseDC((HWND)mw_window, (HDC)mw_deviceContext) != 0)
		{
			printf("Release Device Context Failed.\n");
			//core::shell::ShowErrorMessage("Release Device Context Failed.");
		}
		mw_deviceContext = NIL;*/

		// Destroy device first
		vkDeviceWaitIdle(m_device);
		vkDestroyDevice(m_device, m_allocator);

		// Desotry instance
		vkDestroyInstance(m_instance, m_allocator);
	}

	vkFreeCommandBuffers(m_device, m_graphicsContext->m_commandPool, m_graphicsContext->m_commandBuffers.size(), m_graphicsContext->m_commandBuffers.data());
	vkDestroyCommandPool(m_device, m_graphicsContext->m_commandPool, m_allocator);
	delete m_graphicsContext;
	//vkFreeCommandBuffers(m_device, m_computeContext->m_commandPool, m_computeContext->m_commandBuffers.size(), m_computeContext->m_commandBuffers.data());
	//vkDestroyCommandPool(m_device, m_computeContext->m_commandPool, m_allocator);
	delete m_computeContext;
}

//	rvkCheckRequestedLayers() : Checks requested layers, and spits back the supported ones.
// Only fails if there are no layers on the device at all.
static bool rvkCheckRequestedLayers(std::vector<const char*>& in_layers, std::vector<const char*>& out_layers)
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);
	if (layerCount == 0)
	{	// If no layers were found, we have an issue. We return nothing.
		return false;
	}

	VkLayerProperties *layersAvailable = new VkLayerProperties[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, layersAvailable);

	uint32_t inLayerIndex = 0;
	while (in_layers[inLayerIndex] != NULL)
	{
		// Find the layer in in_layers~
		bool foundLayer = false;
		for (uint32_t i = 0; i < layerCount; ++i)
		{
			if (strcmp(layersAvailable[i].layerName, in_layers[inLayerIndex]) == 0)
			{
				foundLayer = true;
				break;
			}
		}

		// Print out layer if not found
		if (!foundLayer)
		{
			printf("Could not find the layer \"%s\". Ignoring.\n", in_layers[inLayerIndex]);
		}
		// Otherwise save this layer.
		else
		{
			printf("Found layer \"%s\".\n", in_layers[inLayerIndex]);
			out_layers.push_back(in_layers[inLayerIndex]);
		}

		// Go to next layer.
		inLayerIndex++;
	}

	// Add NULL terminator at the end
	out_layers.push_back(NULL);

	return true;
}

//	rvkCheckRequestedExtensions() : Checks that all requested extensions are supported.
// If any are missing, will return a failure.
static bool rvkCheckRequestedExtensions(std::vector<const char*> in_exts)
{
	bool successful = true;

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
	if (extensionCount == 0)
	{
		return false;
	}

	VkExtensionProperties *extensionsAvailable = new VkExtensionProperties[extensionCount];
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionsAvailable);

	uint32_t inExtIndex = 0;
	while (in_exts[inExtIndex] != NULL)
	{
		// Find the layer in in_layers~
		bool foundLayer = false;
		for (uint32_t i = 0; i < extensionCount; ++i)
		{
			if (strcmp(extensionsAvailable[i].extensionName, in_exts[inExtIndex]) == 0)
			{
				foundLayer = true;
				break;
			}
		}

		// Print out layer if not found
		if (!foundLayer)
		{
			printf("Could not find the extension \"%s\". Failing.\n", in_exts[inExtIndex]);
			successful = false;
		}
		// Otherwise save this layer.
		else
		{
			printf("Found extension \"%s\".\n", in_exts[inExtIndex]);
		}

		// Go to next extension.
		inExtIndex++;
	}

	return successful;
}

static void rvkMakeExtensionList ( std::vector<const char*>* requested_extensions )
{
	requested_extensions->push_back("VK_KHR_surface");
	requested_extensions->push_back("VK_KHR_win32_surface");
	requested_extensions->push_back("VK_EXT_debug_report");
	requested_extensions->push_back(NULL); // Add array finisher
}

int gpu::Device::create ( DeviceLayer* layers, uint32_t layerCount )
{
	VkResult result;
	std::vector<const char*> requested_layers;
	std::vector<const char*> actual_layers;
	std::vector<const char*> requested_extensions;

	// Check instance's layers
	{
		// Create the requested layers list
		for (uint32_t iLayer = 0; iLayer < layerCount; ++iLayer)
		{
			// If we're debugging, it's okay to throw in a metric ton of layers.
			// VK_LAYER_LUNARG_standard_validation does exactly that: shorthand adds a ton of layers.
			if (layers[iLayer] == gpu::kDeviceLayerDebug)
				requested_layers.push_back( "VK_LAYER_LUNARG_standard_validation" ); 
		}
		requested_layers.push_back(NULL); // Add array finisher

		// Grab the layers!
		if (!rvkCheckRequestedLayers(requested_layers, actual_layers))
		{
			printf("Could not locate requested instance layers.\n");
			return gpu::kErrorInvalidDevice;
		}
	}

	// Check instance's extensions
	{
		// Create the requested extension list
		rvkMakeExtensionList(&requested_extensions);

		// Grab the extensions!
		if (!rvkCheckRequestedExtensions(requested_extensions))
		{
			printf("Could not locate requested instance extensions.\n");
			return gpu::kErrorInvalidDevice;
		}
	}

	// Create the instance
	{
		VkApplicationInfo applicationInfo;
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = NULL;
		applicationInfo.pApplicationName = kEngineAppName;
		applicationInfo.pEngineName = NULL;
		applicationInfo.engineVersion = kEngineVersion;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo		= &applicationInfo;
		instanceInfo.enabledLayerCount		= actual_layers.size() - 1;
		instanceInfo.ppEnabledLayerNames	= actual_layers.data();
		instanceInfo.enabledExtensionCount	= requested_extensions.size() - 1;
		instanceInfo.ppEnabledExtensionNames= requested_extensions.data();

		result = vkCreateInstance(&instanceInfo, m_allocator, &m_instance);
		if (result != VK_SUCCESS)
		{
			printf("Could not create Vulkan instance (error: %x).\n", (int)result);
			return gpu::kErrorInvalidDevice;
		}

		printf("Created Vulkan instance.\n");
	}

	// Set the the debug callbacks
	{ // TODO: Make this dependant on if the debug extension loaded. The extension may not be available if there are no validation alyers.
		// Load up the functions
		*(void **)&m_extensions.vkCreateDebugReportCallbackEXT	= vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
		*(void **)&m_extensions.vkDestroyDebugReportCallbackEXT	= vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
		*(void **)&m_extensions.vkDebugReportMessageEXT			= vkGetInstanceProcAddr(m_instance, "vkDebugReportMessageEXT");

		// Set up the callback
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		callbackCreateInfo.flags =	VK_DEBUG_REPORT_ERROR_BIT_EXT |
									VK_DEBUG_REPORT_WARNING_BIT_EXT |
									VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = &rvkDebugReportCallback;
		callbackCreateInfo.pUserData = NULL;

		result = m_extensions.vkCreateDebugReportCallbackEXT(m_instance, &callbackCreateInfo, m_allocator, &m_debugCallback);
		if (result != VK_SUCCESS)
		{
			printf("Could not create debug report callback (error: %x).\n", (int)result);
			return gpu::kErrorInvalidDevice;
		}
	}

	return gpu::kError_SUCCESS;
}

//	rvkDebugEnumerateDevices() : Lists all the devices & capabilities on this system.
static void rvkDebugEnumerateDevices(VkInstance& instance, VkSurfaceKHR& surface)
{
	// Base loop through the devices, displaying useful information:

	printf("available devices:\n");

	// Grab the devides
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
	VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

	for (uint32_t i = 0; i < physicalDeviceCount; ++i)
	{
		// Get device properties
		VkPhysicalDeviceProperties deviceProperties = {};
		vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

		// Print device information
		{
			printf("%s, Vendor: %d\n", deviceProperties.deviceName, deviceProperties.vendorID);
			const char* deviceTypes[] = {"OTHER", "INTEGRATED GPU", "DISCRETE GPU", "VIRTUAL GPU", "CPU"};
			printf(" type: %s\n", deviceTypes[deviceProperties.deviceType]);
			printf(" queue priorities: %u\n", deviceProperties.limits.discreteQueuePriorities);
		}

		// Get queues and queue properties
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, NULL);
		VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilyProperties);

		// Print out each queue + it's sub properties
		printf(" queue count: %u\n", queueFamilyCount);
		for (uint32_t j = 0; j < queueFamilyCount; ++j)
		{
			VkBool32 supportsPresent;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, surface, &supportsPresent);

			printf(" queue%u: ", j);
			if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				printf("graphics ");
			if (queueFamilyProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
				printf("compute ");
			if (queueFamilyProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
				printf("transfer ");
			if (queueFamilyProperties[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
				printf("sparse-mem ");
			if (supportsPresent)
				printf("+output ");
			printf("\n");

			//if (supportsPresent && queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			//{
			//	// Save this device for the future, though.
			//	physicalDevice.device = physicalDevices[i];
			//	physicalDevice.properties = deviceProperties;
			//}
		}
		delete[] queueFamilyProperties;
	}
	delete[] physicalDevices;
}

int gpu::Device::initialize ( OutputSurface* surface )
{
	VkResult result;

	// Select the device
	{
		// Grab the devices
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, NULL);
		std::vector<VkPhysicalDevice> physicalDevices (physicalDeviceCount);
		vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

		std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties (physicalDeviceCount);
		std::vector<std::vector<VkQueueFamilyProperties>> physicalDeviceQueueFamilyProperties (physicalDeviceCount);
		std::vector<VkBool32> physicalDeviceSupportsPresent (physicalDeviceCount);
		std::vector<VkBool32> physicalDeviceHasGraphics (physicalDeviceCount);
		std::vector<VkBool32> physicalDeviceHasCompute (physicalDeviceCount);
		std::vector<VkBool32> physicalDeviceHasTransfer (physicalDeviceCount);
		std::vector<std::vector<VkBool32>> physicalDeviceQueueFamilySupportsPresent (physicalDeviceCount);
		uint32_t selectedPhysicalDevice = 0xFFFF;

		// TODO: Make device selection an externally configurable option.

		// Loop through all the physical devices and get their properties.
		for (uint32_t i = 0; i < physicalDevices.size(); ++i)
		{
			// Get device properties
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties[i]);
			// Get device queue properties
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, NULL);
			physicalDeviceQueueFamilyProperties[i].resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, physicalDeviceQueueFamilyProperties[i].data());
			// Get which queues support output
			physicalDeviceQueueFamilySupportsPresent[i].resize(queueFamilyCount);
			physicalDeviceSupportsPresent[i] = VK_FALSE;
			for (uint32_t j = 0; j < physicalDeviceQueueFamilyProperties[i].size(); ++j)
			{
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, surface->m_surface, &physicalDeviceQueueFamilySupportsPresent[i][j]);
				if (physicalDeviceQueueFamilySupportsPresent[i][j]) {
					physicalDeviceSupportsPresent[i] = VK_TRUE;
				}
			}
			// Check the queues for support of graphics, compute, or transfer
			physicalDeviceHasGraphics[i] = VK_FALSE;
			physicalDeviceHasCompute[i] = VK_FALSE;
			physicalDeviceHasTransfer[i] = VK_FALSE;
			for (uint32_t j = 0; j < physicalDeviceQueueFamilyProperties[i].size(); ++j)
			{
				if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					physicalDeviceHasGraphics[i] = true;
				if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VK_QUEUE_COMPUTE_BIT)
					physicalDeviceHasCompute[i] = true;
				if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VK_QUEUE_TRANSFER_BIT)
					physicalDeviceHasTransfer[i] = true;
			}

			// Force to grab at least the first device
			if (selectedPhysicalDevice == 0xFFFF)
			{
				selectedPhysicalDevice = i;
				continue;
			}

			// If the previous selected GPU does not support present, get a new one
			if (physicalDeviceSupportsPresent[selectedPhysicalDevice] == VK_FALSE &&
				physicalDeviceSupportsPresent[i] == VK_TRUE)
			{
				selectedPhysicalDevice = i;
				continue;
			}

			// If the previous selected GPU is not a discrete GPU, then we want to grab a discrete or integrated one.
			// Specifically, we want to avoid software or virtual devices.
			if (physicalDeviceProperties[selectedPhysicalDevice].deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				if (physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					selectedPhysicalDevice = i;
					continue;
				}
			}

			// Below here, we must be the correct type of GPU, so we skip if we're not the right one
			// (there's probably some problem with the logic but hopefully will catch it in testing)
			if ((physicalDeviceProperties[i].deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && physicalDeviceProperties[i].deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				|| physicalDeviceSupportsPresent[i] == VK_FALSE)
			{
				continue;
			}

			// If the previous selected GPU does not support graphics, let's select this new one
			if (physicalDeviceHasGraphics[selectedPhysicalDevice] == VK_FALSE &&
				physicalDeviceHasGraphics[i] == VK_TRUE)
			{
				selectedPhysicalDevice = i;
				continue;
			}

			// If the previous selected GPU does not support compute, let's select it
			if (physicalDeviceHasCompute[selectedPhysicalDevice] == VK_FALSE &&
				physicalDeviceHasCompute[i] == VK_TRUE && physicalDeviceHasGraphics[i] == VK_TRUE)
			{
				selectedPhysicalDevice = i;
				continue;
			}

			// If the previous selected GPU does not support transfer, let's select it
			if (physicalDeviceHasTransfer[selectedPhysicalDevice] == VK_FALSE &&
				physicalDeviceHasTransfer[i] == VK_TRUE && physicalDeviceHasCompute[i] == VK_TRUE && physicalDeviceHasGraphics[i] == VK_TRUE)
			{
				selectedPhysicalDevice = i;
				continue;
			}
		}

		// Make sure we selected a device...
		ARCORE_ASSERT(selectedPhysicalDevice != 0xFFFF);
		ARCORE_ASSERT(physicalDeviceHasGraphics[selectedPhysicalDevice] == VK_TRUE);
		ARCORE_ASSERT(physicalDeviceHasCompute[selectedPhysicalDevice] == VK_TRUE);

		// Select the desired queues now
		m_physicalDevice = physicalDevices[selectedPhysicalDevice];
		m_physicalDeviceProperties = physicalDeviceProperties[selectedPhysicalDevice];

		m_queueIndexGraphics = 0xFFFF;
		m_queueIndexCompute = 0xFFFF;
		m_queueIndexTransfer = 0xFFFF;
		for (int j = 0; j < physicalDeviceQueueFamilyProperties[selectedPhysicalDevice].size(); ++j)
		{
			// Select first compatible graphics queue with present
			if (physicalDeviceQueueFamilySupportsPresent[selectedPhysicalDevice][j] == VK_TRUE
				&& physicalDeviceQueueFamilyProperties[selectedPhysicalDevice][j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (m_queueIndexGraphics == 0xFFFF)
				{
					m_queueIndexGraphics = j;
				}
			}
			// Select first compute queue, reselecting if the queue is the same as the graphics queue
			if (physicalDeviceQueueFamilyProperties[selectedPhysicalDevice][j].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (m_queueIndexCompute == 0xFFFF || m_queueIndexCompute == m_queueIndexGraphics)
				{
					m_queueIndexCompute = j;
				}
			}
			// Select first transfer queue, reselecting if the queue is the same as the graphics queue
			if (physicalDeviceQueueFamilyProperties[selectedPhysicalDevice][j].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (m_queueIndexTransfer == 0xFFFF || m_queueIndexTransfer == m_queueIndexGraphics)
				{
					m_queueIndexTransfer = j;
				}
			}
		}

		// Make sure we selected some queues...
		ARCORE_ASSERT(m_queueIndexGraphics != 0xFFFF);
		ARCORE_ASSERT(m_queueIndexCompute != 0xFFFF);
		ARCORE_ASSERT(m_queueIndexTransfer != 0xFFFF);
	}

	// Create the device
	{
		std::vector<const char*> requested_extensions;
		rvkMakeExtensionList(&requested_extensions);

		// Create queues for graphics & compute:
		VkDeviceQueueCreateInfo queueCreateInfos [3] = {};

		// Graphics queue
		queueCreateInfos[0] = {};
		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].queueFamilyIndex = m_queueIndexGraphics;
		queueCreateInfos[0].queueCount = 1;
		float queuePrioritiesGraphics[] = { 1.0f };   // ask for highest priority for our queue. (range [0,1])
		queueCreateInfos[0].pQueuePriorities = queuePrioritiesGraphics;

		// Compute queue
		queueCreateInfos[1] = {};
		queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[1].queueFamilyIndex = m_queueIndexCompute;
		queueCreateInfos[1].queueCount = 1;
		float queuePrioritiesCompute[] = { 1.0f };   // ask for highest priority for our queue. (range [0,1])
		queueCreateInfos[1].pQueuePriorities = queuePrioritiesCompute;

		// Transfer queue
		queueCreateInfos[2] = {};
		queueCreateInfos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[2].queueFamilyIndex = m_queueIndexTransfer;
		queueCreateInfos[2].queueCount = 1;
		float queuePrioritiesTransfer[] = { 1.0f };   // ask for highest priority for our queue. (range [0,1])
		queueCreateInfos[2].pQueuePriorities = queuePrioritiesTransfer;

		VkDeviceCreateInfo deviceInfo = {};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.queueCreateInfoCount = 3;
		deviceInfo.pQueueCreateInfos = queueCreateInfos;
		deviceInfo.enabledLayerCount = requested_extensions.size();
		deviceInfo.ppEnabledLayerNames = requested_extensions.data();

		const char *deviceExtensions[] = { "VK_KHR_swapchain" };
		deviceInfo.enabledExtensionCount = 1;
		deviceInfo.ppEnabledExtensionNames = deviceExtensions;

		VkPhysicalDeviceFeatures features = {};
		features.shaderClipDistance = VK_TRUE;
		features.geometryShader = VK_TRUE;
		features.tessellationShader = VK_TRUE;
		deviceInfo.pEnabledFeatures = &features;

		result = vkCreateDevice(m_physicalDevice, &deviceInfo, NULL, &m_device);
		if (result != VK_SUCCESS)
		{
			printf("Failed to create logical device (error: %x).\n", (int)result);
			return gpu::kErrorCreationFailed;
		}
	}

	// Create temporary OpenGL context:
	//HGLRC tempContext = wglCreateContext((HDC)mw_deviceContext);
	//if (tempContext == NULL)
	//{
	//	printf("Could not create an OpenGL context: Error code %x.\n", GetLastError());
	//	return gpu::kErrorInvalidDevice;
	//}
	//wglMakeCurrent((HDC)mw_deviceContext, tempContext);

	//// Attempt to load up Windows OGL extensions
	//if (wglgLoadFunctions() < 0)
	//{
	//	printf("Could not load opengl windows module.\n");
	//	wglCreateContextAttribsARB = NULL;
	//	return gpu::kErrorInvalidModule;
	//}
	//if (wglCreateContextAttribsARB == NULL)
	//{
	//	printf("Could not load wglCreateContextAttribsARB. Needed for OpenGL 3.2 and up.\n");
	//	return gpu::kErrorModuleUnsupported;
	//}

	//// Set target attributes to load OpenGL 4.7
	//int target_major = 4;
	//int target_minor = 7;

	//// Load up the correct version of OpenGL
	//mw_renderContext = NIL;
	//do
	//{
	//	// Attribute array
	//	int attribs[] =
	//	{
	//		WGL_CONTEXT_MAJOR_VERSION_ARB, target_major,
	//		WGL_CONTEXT_MINOR_VERSION_ARB, target_minor,
	//		WGL_CONTEXT_FLAGS_ARB, 0,
	//		0
	//	};

	//	// Set up attribs based on input options
	//	attribs[5] = 0;
	//	for (uint32_t i = 0; i < layerCount; ++i)
	//	{
	//		if (layers[i] == gpu::kDeviceLayerDebug)
	//		{	// Enable debug context if a debug "layer" is requested.
	//			attribs[5] |= WGL_CONTEXT_DEBUG_BIT_ARB;
	//		}
	//	}

	//	// Create the new context
	//	mw_renderContext = (intptr_t)wglCreateContextAttribsARB((HDC)mw_deviceContext, 0, attribs);	

	//	// Downgrade version to 4.5 only.
	//	if (mw_renderContext == NIL)
	//	{
	//		target_minor -= 1;
	//		if (target_minor < 5) {
	//			return gpu::kErrorModuleUnsupported;
	//		}
	//	}
	//}
	//while (mw_renderContext == NIL);

	//// Delete the temporary context
	//wglMakeCurrent(NULL, NULL);
	//wglDeleteContext(tempContext);

	//// Set up the render context
	//if (wglMakeCurrent((HDC)mw_deviceContext, (HGLRC)mw_renderContext) == FALSE)
	//{
	//	printf("Could not make new context current. Error code %x.\n", GetLastError());
	//	return gpu::kErrorInvalidModule;
	//}

	//// Load up the OpenGL function pointers:
	//int failed_loads = glgLoadFunctions();
	//if (failed_loads < 0)
	//{
	//	printf("Could not load OpenGL functions.\n");
	//	return gpu::kErrorModuleUnsupported;
	//}
	//else
	//{
	//	printf("OpenGL loaded with %d missing functions.\n", failed_loads);
	//}
	//// Load up the Windows OpenGL function pointers:
	//failed_loads = wglgLoadFunctions();
	//if (failed_loads < 0)
	//{
	//	printf("Could not load OpenGL Windows extension functions.\n");
	//	return gpu::kErrorModuleUnsupported;
	//}
	//else
	//{
	//	printf("OpenGL Windows extension loaded with %d missing functions.\n", failed_loads);
	//}

	//// Clear out errors now
	//while ( glGetError() != 0 ) { ; }

	//// Query version number:
	//int OpenGLVersion[2] = {0, 0};
	//glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	//glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	//printf("Created an OpenGL %d.%d context\n", OpenGLVersion[0], OpenGLVersion[1]);

	//// Query vendor and renderer:
	//const uchar* vendor		= glGetString(GL_VENDOR);
	//const uchar* renderer	= glGetString(GL_RENDERER);
	//printf("device vendor: %s\n", vendor);
	//printf("     renderer: %s\n", renderer);

	//// Update the target device:
	//m_TargetDisplayDevice = this;

	//// Enable debug "layer"
	//for (uint32_t i = 0; i < layerCount; ++i)
	//{
	//	if (layers[i] == gpu::kDeviceLayerDebug)
	//	{	
	//		glDebugMessageCallback(DebugCallback, this);
	//	}
	//}

	return gpu::kError_SUCCESS;
}

int gpu::Device::refresh ( intptr_t module_handle, intptr_t module_window )
{
	// Destroy the device context
	//if (mw_deviceContext != NIL)
	//{
	//	if (ReleaseDC((HWND)mw_window, (HDC)mw_deviceContext) != 1)
	//	{
	//		printf("Release Device Context Failed.\n");
	//		return gpu::kErrorInvalidDevice;
	//	}
	//	mw_deviceContext = NIL;
	//}

	//// Update context
	//mw_module = module_handle;
	//mw_window = module_window;

	//// Create new context and associate OpenGL with it
	//mw_deviceContext = (intptr_t)GetDC((HWND)mw_window);
	//if (mw_deviceContext == NIL)
	//{
	//	printf("Could not create a device context.\n");
	//	return gpu::kErrorInvalidDevice;
	//}

	//// Set up the render context
	//if (wglMakeCurrent((HDC)mw_deviceContext, (HGLRC)mw_renderContext) != TRUE)
	//{
	//	printf("Could not make new context current.\n");
	//	return gpu::kErrorInvalidModule;
	//}

	return gpu::kError_SUCCESS;
}

gpu::GraphicsContext* gpu::Device::getContext ( void )
{
	VkResult result;
	if (m_graphicsContext == NULL)
	{
		m_graphicsContext = new gpu::GraphicsContext();

		// Set up the graphics context:
		VkCommandPoolCreateInfo command_pool_create_info = {};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_create_info.queueFamilyIndex = m_queueIndexGraphics;
		result = vkCreateCommandPool(m_device, &command_pool_create_info, NULL, &m_graphicsContext->m_commandPool);
		ARCORE_ASSERT(result == VK_SUCCESS);

		m_graphicsContext->m_commandBuffers.resize(5, VK_NULL_HANDLE);

		VkCommandBufferAllocateInfo command_buffer_allocate_info {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = m_graphicsContext->m_commandPool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = m_graphicsContext->m_commandBuffers.size();
		result = vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, m_graphicsContext->m_commandBuffers.data());
		ARCORE_ASSERT(result == VK_SUCCESS);

		// Give it this device for referencing needed items?
		//m_graphicsContext->m_device = this;

	}
	return m_graphicsContext;
}

gpu::ComputeContext* gpu::Device::getComputeContext ( void )
{
	if (m_computeContext == NULL)
	{
		m_computeContext = new gpu::ComputeContext();
	}
	return m_computeContext;
}

// Grab native device object
void* gpu::Device::getNative ( void )
{
	return m_device;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
rvkDebugReportCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
	int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	ARCORE_ASSERT((flags | VK_DEBUG_REPORT_ERROR_BIT_EXT) == 0);
	OutputDebugStringA(pLayerPrefix);
	OutputDebugStringA(" ");
	OutputDebugStringA(pMessage);
	OutputDebugStringA("\n");
	return VK_FALSE;
}

#endif