#ifndef WINDOWING_RRWINDOW_H_
#define WINDOWING_RRWINDOW_H_

#define VK_USE_PLATFORM_WIN32_KHR

#include "core/os.h"
#include "core/types.h"
#include "vulkan/vulkan.h"

class RrWindow
{
public:
	explicit RrWindow(
		HINSTANCE	hInstance,
		HINSTANCE	hPrevInstance,
		LPSTR		lpCmdLine,
		int			nCmdShow );

	virtual ~RrWindow ( void );

	bool UpdateMessages ( void );

	bool IsDone ( void );
	bool IsActive ( void );

private:

	void CreateScreen ( void );
	void CreateConsole ( void );
	void CreateGfxInstance ( void );
	void CreateGfxSurface ( void );
	void CreateGfxSwapchain ( void );

	void DestroyScreen ( void );
	void DestroyGfxInstance ( void );
	void DestroyGfxSurface ( void );

private:
	friend LRESULT CALLBACK MessageUpdate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int m_width;
	int m_height;
	bool m_fullscreen;

	// OS:

	HINSTANCE	mw_instance;
	HINSTANCE	mw_previnstance;
	LPSTR		mw_cmdline;
	int			mw_cmdshow;

	HWND		mw_window;

	// Gfx:

	VkInstance			mvk_instance;
	VkPhysicalDevice*	mvk_available_devices;
	VkDevice			mvk_device;
	VkSurfaceKHR		mvk_surface;

	VkCommandPool		mvk_command_pool;

	size_t				mvk_node_index;
	VkFormat			mvk_format;
	VkColorSpaceKHR		mvk_colorspace;
	VkSwapchainKHR		mvk_swapchain;
	size_t				mvk_swapchain_image_count;
	VkCommandBuffer*	mvk_swapchain_image_command_buffers;
	VkImage*			mvk_swapchain_images;
	VkImageView*		mvk_swapchain_image_views;

	// Windows Message Loop
	MSG		msg;
	bool	done;
	bool	active;
	bool	focused;
};

#endif//WINDOWING_RRWINDOW_H_