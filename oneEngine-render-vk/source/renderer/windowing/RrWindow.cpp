
#include "RrWindow.h"

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#define ERROR_OUT(_str, ...) { printf(_str, __VA_ARGS__); abort(); }

// Message loop
LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//===============================================================================================//
// WINDOW CLASS:
//===============================================================================================//

static RrWindow* active_window = NULL;

RrWindow::RrWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	: mw_instance(hInstance), mw_previnstance(hPrevInstance), mw_cmdline(lpCmdLine), mw_cmdshow(nCmdShow),
	mw_window(0),
	done(false), active(true), focused(true)
{
	active_window = this;

	m_width = 1280;
	m_height = 720;
	m_fullscreen = false;

	// Load window options 
	CGameSettings::Active()->LoadSettings();

	// Create everything!
	CreateScreen();
	CreateConsole();
	CreateGfxInstance();
	CreateGfxSurface();
	CreateGfxSwapchain();
}

RrWindow::~RrWindow(void)
{
	DestroyGfxSurface();
	DestroyGfxInstance();
	DestroyScreen();
}

void RrWindow::CreateConsole ( void )
{
#	ifdef _WIN32

	// Get a console for this program
	AllocConsole();

#		if _MSC_VER < 1900

	int hConHandle;
	long lStdHandle;
	FILE *fp;

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();

#		else

	// Redirect STDOUT to console
	FILE* pCout;
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	// Redirect STDIN from console
	FILE* pCin;
	freopen_s(&pCin,  "CONIN$",  "r", stdin );
	// Redirect STDERR to console
	FILE* pCerr;
	freopen_s(&pCerr, "CONOUT$", "w", stderr);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
	std::ios::sync_with_stdio();

#		endif

	// Move console to the side
	HWND hConsole;
	hConsole = GetConsoleWindow();
	SetWindowPos(hConsole, 0, 0, 16, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

#	elif 
	/// use openpty to create a terminal window and redirect output to it
#	endif
}



void setImageLayout(
	VkCommandBuffer commandBuffer,
	VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout)
{
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	// Undefined layout:
	//   Note: Only allowed as initial layout!
	//   Note: Make sure any writes to the image have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED)
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

	// Old layout is color attachment:
	//   Note: Make sure any writes to the color buffer have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Old layout is transfer source:
	//   Note: Make sure any reads from the image have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	// Old layout is shader read (sampler, input attachment):
	//   Note: Make sure any shader reads from the image have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;

	// New layout is transfer destination (copy, blit):
	//   Note: Make sure any copyies to the image have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	// New layout is transfer source (copy, blit):
	//   Note: Make sure any reads from and writes to the image have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	// New layout is color attachment:
	//   Note: Make sure any writes to the color buffer hav been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	// New layout is depth attachment:
	//   Note: Make sure any writes to depth/stencil buffer have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// New layout is shader read (sampler, input attachment):
	//   Note: Make sure any writes to the image have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	// Put barrier inside the setup command buffer
	vkCmdPipelineBarrier(commandBuffer,
		// Put the barriers for source and destination on
		// top of the command buffer
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, NULL,
		0, NULL,
		1, &imageMemoryBarrier);
}

//===============================================================================================//
// RRWINDOW INIT:
//===============================================================================================//

//	CreateScreen
// Creates OS specific rendering surface
void RrWindow::CreateScreen ( void )
{
	uint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left		= 0;
	WindowRect.right	= m_width;
	WindowRect.top		= 0;
	WindowRect.bottom	= m_height;

	m_fullscreen = false;

	//mw_instance			= GetModuleHandle(NULL);			// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) MessageUpdate;				// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= mw_instance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "GraphicsWindow";						// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		ERROR_OUT("Failed To Register The Window Class. (Has the class already been registered?)\n");
	}

	if (m_fullscreen)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= m_width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			ERROR_OUT("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card.");
		}

		// Set window style
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	// Adjust Window To True Requested Size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	mw_window = CreateWindowEx( dwExStyle,	// Extended Style For The Window
		"GraphicsWindow",					// Class Name
		"Not an OpenGL Window",				// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		64, 64,								// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		mw_instance,						// Instance
		NULL								// Dont Pass Anything To WM_CREATE
	);
	if ( mw_window == NIL )
	{
		ERROR_OUT("Window Creation Error.");
	}

	// Show the window now
	ShowWindow(mw_window, SW_SHOW);
	SetForegroundWindow(mw_window);
	SetFocus(mw_window);
}

//	CreateGfxInstance
// Creates an instance of the device used for rendering
void RrWindow::CreateGfxInstance ( void )
{
	std::vector<const char*> extensions;
	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	VkResult result;
	VkApplicationInfo applicationInfo;
	VkInstanceCreateInfo instanceInfo;

	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = NULL;
	applicationInfo.pApplicationName = "Vulkan Test";
	applicationInfo.pEngineName = "1Engine";
	applicationInfo.engineVersion = 0;
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &applicationInfo;
	// Don't enable any layers
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = NULL;
	// Don't enable any extensions
	instanceInfo.enabledExtensionCount = extensions.size();
	instanceInfo.ppEnabledExtensionNames = &extensions[0];

	// Create the instance
	result = vkCreateInstance(&instanceInfo, NULL, &mvk_instance);
	if (result != VK_SUCCESS)
	{
		ERROR_OUT("Failed to create instance: %d\n", result);
	}


	// Query how many devices are present in the system
	uint32_t deviceCount = 0;
	result = vkEnumeratePhysicalDevices(mvk_instance, &deviceCount, NULL);
	if (result != VK_SUCCESS) ERROR_OUT("Failed to query the number of physical devices present: %d\n", result);

	// There has to be at least one device present
	if (deviceCount == 0) ERROR_OUT("Couldn't detect any device present with Vulkan support: %d\n", result);

	// Get the physical devices
	mvk_available_devices = new VkPhysicalDevice[deviceCount];
	result = vkEnumeratePhysicalDevices(mvk_instance, &deviceCount, mvk_available_devices);
	if (result != VK_SUCCESS) ERROR_OUT("Faied to enumerate physical devices present: %d\n", result);

	// Enumerate all physical devices
	VkPhysicalDeviceProperties deviceProperties;
	for (uint32_t i = 0; i < deviceCount; i++)
	{
		memset(&deviceProperties, 0, sizeof deviceProperties);
		vkGetPhysicalDeviceProperties(mvk_available_devices[i], &deviceProperties);
		printf("Driver Version: %d\n", deviceProperties.driverVersion);
		printf("Device Name:    %s\n", deviceProperties.deviceName);
		printf("Device Type:    %d\n", deviceProperties.deviceType);
		printf("API Version:    %d.%d.%d\n",
			VK_VERSION_MAJOR(deviceProperties.apiVersion),
			VK_VERSION_MINOR(deviceProperties.apiVersion),
			VK_VERSION_PATCH(deviceProperties.apiVersion)
		);
	}

	// Now we need to create the device to use:

	VkDeviceQueueCreateInfo deviceQueueInfo;
	deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueInfo.pNext = NULL;
	deviceQueueInfo.flags = 0;
	// Use the first queue family in the family list
	deviceQueueInfo.queueFamilyIndex = 0;
	// Set up only 1 queue
	float queuePriorities[] = { 1.0f };
	deviceQueueInfo.queueCount = 1;
	deviceQueueInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = NULL;
	deviceInfo.flags = 0;
	// No extensions or layers
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = NULL;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.ppEnabledExtensionNames = NULL;
	// Fuck features for now
	deviceInfo.pEnabledFeatures = NULL;
	// Set up queue info
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &deviceQueueInfo;

	// Create a logical device from the 0th device
	result = vkCreateDevice(mvk_available_devices[0], &deviceInfo, NULL, &mvk_device);
	if (result != VK_SUCCESS) ERROR_OUT("Failed creating logical device: %d\n", result);
}

void RrWindow::CreateGfxSurface ( void )
{
	VkResult result;

	// Create surface
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = mw_instance;
		surfaceCreateInfo.hwnd = mw_window;
		// Create the surface
		result = vkCreateWin32SurfaceKHR(mvk_instance, &surfaceCreateInfo, NULL, &mvk_surface);
		if (result != VK_SUCCESS) ERROR_OUT("Failed to create Vulkan surface: %d\n", result);
	}

	// Check each render queue we have for the setup that we can render to it:
	{
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(mvk_available_devices[0], &queueCount, NULL);
		std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(mvk_available_devices[0], &queueCount, &queueProperties[0]);

		printf("There are %d queues available:\n", queueCount);
		for (uint32_t j = 0; j < queueCount; j++)
		{
			printf("\t%d: Count of Queues: %d\n", j, queueProperties[j].queueCount);
			printf("\tSupported operations on this queue:\n");
			if (queueProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				printf("\t\t Graphics\n");
			if (queueProperties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
				printf("\t\t Compute\n");
			if (queueProperties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
				printf("\t\t Transfer\n");
			if (queueProperties[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
				printf("\t\t Sparse Binding\n");
		}

		std::vector<VkBool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
		{	// Does it support present?
			vkGetPhysicalDeviceSurfaceSupportKHR(mvk_available_devices[0], i, mvk_surface, &supportsPresent[i]);
		}

		uint32_t graphicIndex = UINT32_MAX;
		uint32_t presentIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				if (graphicIndex == UINT32_MAX)
					graphicIndex = i;
				if (supportsPresent[i] == VK_TRUE)
				{	// Does it support graphics AND present? Then select it.
					graphicIndex = i;
					presentIndex = i;
					break;
				}
			}
		}
		// If there is no queue that supports both present and graphics, try and find a separate present queue.
		if (presentIndex == UINT32_MAX)
		{
			for (uint32_t i = 0; i < queueCount; i++)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentIndex = i;
					break;
				}
			}
		}
		// Cannot render if we don't have both kinds of queues
		if (graphicIndex == UINT32_MAX || presentIndex == UINT32_MAX) ERROR_OUT("Do not have both a rendering and present queue.\n");
		if (graphicIndex != presentIndex) ERROR_OUT("Do not support separated rendering and presenting queues.\n");

		mvk_node_index = graphicIndex;
		printf("Selected queue #%d for rendering.\n", graphicIndex);
	}
	
	// Get number of formats
	{
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mvk_available_devices[0], mvk_surface, &formatCount, NULL);
		// Get the formats
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mvk_available_devices[0], mvk_surface, &formatCount, &surfaceFormats[0]);

		// Save the color space and format that we want:
		if ( formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED )
			// If the format list includes just one entry of VK_FORMAT_UNDEFINED, the surface has no preferred format.
			mvk_format = VK_FORMAT_B8G8R8A8_UNORM;
		else if ( formatCount >= 1 )
			// Otherwise, at least one format will be returned
			mvk_format = surfaceFormats[0].format;
		else 
			ERROR_OUT("No color formats returned.\n");
		mvk_colorspace = surfaceFormats[0].colorSpace;
	}
}

void RrWindow::CreateGfxSwapchain ( void )
{
	printf("Creating Vulkan swapchain...\n");

	VkResult result;

	// Get physical device surface properties and formats:
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mvk_available_devices[0], mvk_surface, &surfaceCapabilities);
	if (result != VK_SUCCESS) ERROR_OUT("Failed getting surface capabilities: %d\n", result);

	// Get available presentation modes:
	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mvk_available_devices[0], mvk_surface, &presentModeCount, NULL);
	if (result != VK_SUCCESS) ERROR_OUT("Failed getting available presentation modes: %d\n", result);
	if (presentModeCount == 0) ERROR_OUT("No presentation modes returned.\n");

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(mvk_available_devices[0], mvk_surface, &presentModeCount, &presentModes[0]);
	if (result != VK_SUCCESS) ERROR_OUT("Failed getting available presentation modes: %d\n", result);

	// Set up extents for the window:
	VkExtent2D swapchainExtent = { };
	if (surfaceCapabilities.currentExtent.width == -1)
	{
		swapchainExtent.width = m_width;
		swapchainExtent.height = m_height;
		printf("\tExtents provided by application.\n");
	}
	else
	{
		swapchainExtent = surfaceCapabilities.currentExtent;
		m_width = surfaceCapabilities.currentExtent.width;
		m_height = surfaceCapabilities.currentExtent.height;
		printf("\tExtents provided by device: %dx%d.\n", m_width, m_height);
	}

	// Select a present mode for rendering
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (uint32_t i = 0; i < presentModeCount; i++)
	{
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{	// Prefer mailbox mode if present
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if (presentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode != VK_PRESENT_MODE_FIFO_KHR)
		{	// Fall back to (shudders) immediate mode
			presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// Determine the number of images for our swapchain
	uint32_t desiredImages = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 && desiredImages > surfaceCapabilities.maxImageCount)
	{
		desiredImages = surfaceCapabilities.maxImageCount;
	}

	// ??? No idea yet
	VkSurfaceTransformFlagsKHR preTransform = surfaceCapabilities.currentTransform;
	if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

	// Create the swapchain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.pNext = NULL;

	swapChainCreateInfo.surface = mvk_surface;
	swapChainCreateInfo.minImageCount = desiredImages;
	swapChainCreateInfo.imageFormat = mvk_format;
	swapChainCreateInfo.imageColorSpace = mvk_colorspace;
	swapChainCreateInfo.imageExtent = swapchainExtent;
	// Create Vulkan's version of GL_COLOR_ATTACHMENT0:
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.queueFamilyIndexCount = 0;
	swapChainCreateInfo.pQueueFamilyIndices = NULL;
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = true;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Opaque shit

	result = vkCreateSwapchainKHR(mvk_device, &swapChainCreateInfo, NULL, &mvk_swapchain);
	if (result != VK_SUCCESS) ERROR_OUT("Could not create the swapchain: %d\n", result);


	// Get presentable texture handles
	{
		uint32_t imageCount;
		result = vkGetSwapchainImagesKHR(mvk_device, mvk_swapchain, &imageCount, NULL);
		if (result != VK_SUCCESS) ERROR_OUT("Failed querying swapchain images: %d\n", result);
		if (imageCount == 0) ERROR_OUT("No swapchain images returned: %d\n", result);

		mvk_swapchain_images = new VkImage [imageCount];
		result = vkGetSwapchainImagesKHR(mvk_device, mvk_swapchain, &imageCount, mvk_swapchain_images);
		if (result != VK_SUCCESS) ERROR_OUT("Failed querying swapchain images: %d\n", result);

		mvk_swapchain_image_count = imageCount;
	}

	// Make an image view for each texture
	for (uint32_t i = 0; i < mvk_swapchain_image_count; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = { };
		colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colorAttachmentView.pNext = NULL;
		colorAttachmentView.format = mvk_format;
		colorAttachmentView.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorAttachmentView.subresourceRange.baseMipLevel = 0;
		colorAttachmentView.subresourceRange.levelCount = 1;
		colorAttachmentView.subresourceRange.baseArrayLayer = 0;
		colorAttachmentView.subresourceRange.layerCount = 1;
		colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorAttachmentView.flags = 0;

		// Transform images from the initial (undefined) layer to present layout
		/*setImageLayout(mvk_swapchain_image_command_buffers[i], mvk_swapchain_images[i],
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		colorAttachmentView.image = mvk_swapchain_images[i];

		// Create the view
		result = vkCreateImageView(mvk_device, &colorAttachmentView, NULL, &mvk_swapchain_image_views[i]);
		if (result != VK_SUCCESS) ERROR_OUT("Could not create image view: %d\n", result);*/
	}


	// Create the command buffer(s)
	{
		// Create the command pool first:
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = mvk_node_index;
		poolInfo.flags = 0;

		result = vkCreateCommandPool(mvk_device, &poolInfo, nullptr, &mvk_command_pool);
		if (result != VK_SUCCESS) ERROR_OUT("Failed to create command pool: %d\n", result);

		// Create the buffer now
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mvk_command_pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = mvk_swapchain_image_count;

		mvk_swapchain_image_command_buffers = new VkCommandBuffer [mvk_swapchain_image_count];
		result = vkAllocateCommandBuffers(mvk_device, &allocInfo, mvk_swapchain_image_command_buffers);
		if (result != VK_SUCCESS) ERROR_OUT("Failed to allocate command buffers: %d\n", result);

		printf("%d Command buffers created.\n", mvk_swapchain_image_count);

		// Create a render pass??
		/*VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent;*/
	}


}



void RrWindow::DestroyScreen ( void )
{
	DestroyWindow(mw_window);
}

void RrWindow::DestroyGfxInstance ( void )
{
	delete [] mvk_available_devices;

	vkDestroyDevice(mvk_device, NULL);
	vkDestroyInstance(mvk_instance, NULL);
}

void RrWindow::DestroyGfxSurface ( void )
{
	vkFreeCommandBuffers(mvk_device, mvk_command_pool, mvk_swapchain_image_count, mvk_swapchain_image_command_buffers);
	delete [] mvk_swapchain_image_command_buffers;
	vkDestroyCommandPool(mvk_device, mvk_command_pool, NULL);
	vkDestroySwapchainKHR(mvk_device, mvk_swapchain, NULL);
}



//===============================================================================================//
// RRWINDOW STATE HANDLING:
//===============================================================================================//

bool RrWindow::UpdateMessages ( void )
{
	// Is There A Message Waiting?
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{	// Have We Received A Quit Message?
		if (msg.message==WM_QUIT)
		{	// Then done
			done = true;							
		}
		// If Not, Deal With Window Messages
		else
		{	// Translate The Message
			TranslateMessage(&msg);			
			// Dispatch The Message
			DispatchMessage(&msg);
		}
		return false;
	}
	return active;
}
bool RrWindow::IsDone ( void )
{
	return done;
}
bool RrWindow::IsActive ( void )
{
	return active;
}


//===============================================================================================//
// WINDOWS MESSAGE LOOP:
//===============================================================================================//

static void WndSetMouseClip ( HWND hWnd, bool & hiddencursor )
{
	/*if (   ( CInput::SysMouseX() == std::min<long>( std::max<long>( CInput::SysMouseX(), 0 ), Screen::Info.width ) )
		&& ( CInput::SysMouseY() == std::min<long>( std::max<long>( CInput::SysMouseY(), 0 ), Screen::Info.height ) ))*/
	if ( false )
	{
		hiddencursor = false;
		RECT rc;
		GetClientRect( hWnd, &rc );
		POINT xy, wh;
		xy.x = rc.left;
		xy.y = rc.top;
		wh.x = rc.right;
		wh.y = rc.bottom;
		ClientToScreen( hWnd, &xy );
		ClientToScreen( hWnd, &wh );
		rc.left = xy.x;
		rc.top = xy.y;
		rc.right = wh.x;
		rc.bottom = wh.y;
		ClipCursor( &rc );
	}
}

LRESULT CALLBACK MessageUpdate(
	HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	static bool hiddencursor = false;
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_ACTIVATE:							// Watch For Window Activate Message
	{
		if (!HIWORD(wParam))					// Check Minimization State
		{
			active_window->active = true;	// Program Is Active
		}
		else
		{
			active_window->active = false;	// Program Is No Longer Active
			active_window->focused = false;	// Program is no longer focused
		}

		return 0;								// Return To The Message Loop
	}
	case WM_SETFOCUS:
	{
		active_window->focused = true;	// Program is no longer focused
		WndSetMouseClip( hWnd, hiddencursor );
		return 0;
	}
	case WM_KILLFOCUS:
		active_window->focused = false;	// Program is no longer focused
		return 0;
	case WM_MOVE:
	{
		if ( active_window->focused )
		{
			hiddencursor = false;
			POINT pt;
			GetCursorPos( &pt );
			ScreenToClient( hWnd, &pt );
			/*CInput::_sysMouseX( pt.x );
			CInput::_sysMouseY( pt.y );*/
			WndSetMouseClip( hWnd, hiddencursor );
		}
		return 0;
	}

	case WM_SYSCOMMAND:							// Intercept System Commands
	{
		switch (wParam)							// Check System Calls
		{
		case SC_SCREENSAVE:					// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
		case SC_KEYMENU:					// ALT-Key nonsense?
			return 0;							// Prevent From Happening
		}
		break;									// Exit
	}

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}

	// Mouse movement
	case WM_MOUSEMOVE:
	{
		if ( active_window->focused )
		{
			/*CInput::_sysMouseX( LOWORD(lParam) );
			CInput::_sysMouseY( HIWORD(lParam) );*/
		}
		return 0;
	}
	case WM_SETCURSOR:
	{ //http://stackoverflow.com/questions/5629613/hide-cursor-in-client-rectangle-but-not-on-title-bar
		WORD ht = LOWORD(lParam);
		if (HTCLIENT==ht && !hiddencursor)
		{
			hiddencursor = true;
			ShowCursor(false);
		}
		else if (HTCLIENT!=ht && hiddencursor) 
		{
			hiddencursor = false;
			ShowCursor(true);
		}
	}
	break;
	case WM_INPUT: 
		if ( wParam == RIM_INPUT )
		{
			UINT dwSize = 40;
			static BYTE lpb[40];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
				lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if ((raw->header.dwType == RIM_TYPEMOUSE) && ( active_window->focused ))
			{
				/*CInput::_addRawMouseX( raw->data.mouse.lLastX );
				CInput::_addRawMouseY( raw->data.mouse.lLastY );*/

				//return 0;
				return DefWindowProc(hWnd,uMsg,wParam,lParam);
			} 
			else if ((raw->header.dwType == RIM_TYPEKEYBOARD) && ( active_window->focused ))
			{
				ushort vkey		= raw->data.keyboard.VKey;
				ushort flags	= raw->data.keyboard.Flags;
				ushort scankey	= raw->data.keyboard.MakeCode;

				if ( flags&RI_KEY_BREAK )
				{
					/*CInput::_keyup(_inputtable[vkey], true);
					CInput::_key(_inputtable[vkey], false);*/
					//SendMessage( hWnd, WM_KEYUP, vkey, 0 );
				}
				else // is a make
				{
					/*CInput::_keydown(_inputtable[vkey], true);
					CInput::_key(_inputtable[vkey], true);*/
					//SendMessage( hWnd, WM_KEYDOWN, vkey, 0 );
				}

				/*{
				INPUT inputs [1];
				memset( inputs, 0, sizeof(INPUT) * 1 );
				inputs[0].type = INPUT_KEYBOARD;
				inputs[0].ki.wVk = vkey;
				inputs[0].ki.dwFlags = flags;
				inputs[0].ki.wScan = scankey;
				SendInput( 1, inputs, sizeof(INPUT) );
				}*/
				//return DefWindowProc(hWnd,uMsg,wParam,lParam);

				//return 0;
				//return DefWindowProc(hWnd,uMsg,wParam,lParam);
			}
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
		break;

		// Mouse Buttons
	case WM_LBUTTONDOWN:
	{
		/*CInput::_mousedown(CInput::MBLeft,true);
		CInput::_mouse(CInput::MBLeft,true);*/

		WndSetMouseClip( hWnd, hiddencursor );
		return 0;
	}
	case WM_LBUTTONUP:
	{
		/*CInput::_mouseup(CInput::MBLeft,true);
		CInput::_mouse(CInput::MBLeft,false);*/
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		/*CInput::_mousedown(CInput::MBRight,true);
		CInput::_mouse(CInput::MBRight,true);*/

		WndSetMouseClip( hWnd, hiddencursor );
		return 0;
	}
	case WM_RBUTTONUP:
	{
		/*CInput::_mouseup(CInput::MBRight,true);
		CInput::_mouse(CInput::MBRight,false);*/
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		/*CInput::_mousedown(CInput::MBMiddle,true);
		CInput::_mouse(CInput::MBMiddle,true);*/

		WndSetMouseClip( hWnd, hiddencursor );
		return 0;
	}
	case WM_MBUTTONUP:
	{
		/*CInput::_mouseup(CInput::MBMiddle,true);
		CInput::_mouse(CInput::MBMiddle,false);*/
		return 0;
	}
	case WM_XBUTTONDOWN:
	{
		/*CInput::_mousedown(CInput::MBXtra,true);
		CInput::_mouse(CInput::MBXtra,true);*/

		WndSetMouseClip( hWnd, hiddencursor );
		return 0;
	}
	case WM_XBUTTONUP:
	{
		/*CInput::_mouseup(CInput::MBXtra,true);
		CInput::_mouse(CInput::MBXtra,false);*/
		return 0;
	}

	// Mouse wheel
	case WM_MOUSEWHEEL:
	{
		/*CInput::_currMouseW( GET_WHEEL_DELTA_WPARAM(wParam) );*/
		return 0;
	}

	//case WM_TOUCH:
	//{
	//	Debug::Console->PrintWarning( "Touch message\n" );
	//	return 0;
	//}
	//case WM_GESTURENOTIFY:
	//{
	//	Debug::Console->PrintWarning( "Gesture notify message\n" );
	//	return 0;
	//}
	//case WM_GESTURE:
	//{
	//	Debug::Console->PrintWarning( "Gesture message\n" );
	//	return 0;
	//}
	//case WM_HSCROLL:
	//{
	//	Debug::Console->PrintWarning( "HScroll message\n" );
	//	return 0;
	//}
	//case WM_VSCROLL:
	//{
	//	Debug::Console->PrintWarning( "VScroll message\n" );
	//	return 0;
	//}

	/*case WM_SIZE:								// Resize The OpenGL Window
	{
		hiddencursor = false;
		RrWindow::ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}*/

	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}