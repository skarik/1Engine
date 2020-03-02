#ifndef GPU_WRAPPER_RENDER_TARGET_H_
#define GPU_WRAPPER_RENDER_TARGET_H_

#include "./Texture.dx11.h"
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

		//	create( device ) : Creates a collection of render targets on given device
		GPUW_API int			create ( Device* device );
		GPUW_API int			destroy ( Device* device );

		//	attach( slot, texture ) : Throws the given texture into the slot
		GPUW_API int			attach ( int slot, Texture* texture );
		//	attach( slot, buffer ) : Throws the given buffer into the slot
		GPUW_API int			attach ( int slot, WOFrameAttachment* buffer );
		//	assemble() : Validates all the collection of textures & buffers and "assembles" into a valid RenderTarget collection
		GPUW_API int			assemble ( void );

		GPUW_API bool			empty ( void );
		GPUW_API bool			valid ( void );

		GPUW_API gpuHandle		nativePtr ( void );

		GPUW_API Texture*		getAttachment ( int slot );
		GPUW_API WOFrameAttachment*
								getWOAttachment ( int slot );

	private:
		friend GraphicsContext;
		friend OutputSurface;

		void*							m_highlevelAttachments [18];

		void*							m_attachments [16];
		void*							m_attachmentDepthStencil;
		bool							m_assembled;
		bool							m_hasFailure;
	};
};

#endif//GPU_WRAPPER_RENDER_TEXTURES_H_