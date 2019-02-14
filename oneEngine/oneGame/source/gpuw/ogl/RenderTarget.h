#ifndef GPU_WRAPPER_RENDER_TARGET_H_
#define GPU_WRAPPER_RENDER_TARGET_H_

#include "./Texture.h"
#include "gpuw/Public/Slots.h"

namespace gpu
{
	class Device;
	class Texture;
	class GraphicsContext;
	class OutputSurface;

	class RenderTarget
	{
	public:
		GPUW_API explicit		RenderTarget ( void );
		GPUW_API				~RenderTarget ( void );

		GPUW_API int			create ( Device* device );

		GPUW_API int			attach ( int slot, Texture* texture );
		GPUW_API int			attach ( int slot, WOFrameAttachment* buffer );

		GPUW_API int			destroy ( Device* device );

		GPUW_API bool			empty ( void );
		GPUW_API bool			valid ( void );

		GPUW_API gpuHandle		nativePtr ( void );

		GPUW_API Texture*		getAttachment ( int slot );
		GPUW_API WOFrameAttachment*
								getWOAttachment ( int slot );

		// validates
		GPUW_API int			assemble ( void );

	private:
		friend GraphicsContext;
		friend OutputSurface;

		unsigned int					m_framebuffer;
		void*							m_attachments [16];
		void*							m_attachmentDepth;
		void*							m_attachmentStencil;
		bool							m_attachmentIsTexture [16];
		bool							m_assembled;
		//core::gfx::tex::arTextureType	m_type;
	};
};

#endif//GPU_WRAPPER_RENDER_TEXTURES_H_