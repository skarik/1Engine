#ifndef GPU_WRAPPER_RENDER_TEXTURES_H_
#define GPU_WRAPPER_RENDER_TEXTURES_H_

#include "renderer/gpuw/Textures.h"

namespace gpu
{
	enum RenderTargetSlot
	{
		kRenderTargetSlotColor0 = 0,
		kRenderTargetSlotDepth = -1,
		kRenderTargetSlotStencil = -2,
	};
	class Device;
	class RenderTarget
	{
	public:
		RENDER_API explicit		RenderTarget ( void );
		RENDER_API				~RenderTarget ( void );

		RENDER_API int			create ( Device* device );

		RENDER_API int			attach ( int slot, Texture* texture );
		RENDER_API int			attach ( int slot, WOFrameAttachment* buffer );

		RENDER_API int			destroy ( Device* device );

		RENDER_API bool			empty ( void );
		RENDER_API bool			valid ( void );

		RENDER_API gpuHandle	nativePtr ( void );

		// validates
		RENDER_API int			assemble ( void );

	private:
		unsigned int					m_framebuffer;
		void*							m_attachments [18];
		//core::gfx::tex::arTextureType	m_type;
	};
};

#endif//GPU_WRAPPER_RENDER_TEXTURES_H_