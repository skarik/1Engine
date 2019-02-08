//===============================================================================================//
//
//		Dusk UI : class UIRenderer
//
// UIRenderer is the main renderer class. It's instantiated internally by UserInterface.
// All style options are configured through the UIRenderer object.
//
//===============================================================================================//
#ifndef ENGINE_COMMON_DUSK_UI_RENDERER_H_
#define ENGINE_COMMON_DUSK_UI_RENDERER_H_

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class RrFontTexture;
class rrMeshBuffer;
/*namespace gpu
{
	class RenderTarget;
	class Texture;
}*/

namespace Dusk
{
	class UserInterface;
	class Element;

	class UIRenderer : public CRenderableObject
	{
	public:
		explicit				UIRenderer ( UserInterface* ui );
								~UIRenderer ( void );

		//	PreRender() : Called before the internal render-loop executes.
		// Can be called multiple times per frame, but generally only once per camera.
		// Use to calculate transformation matrices w/ the given camera before sending to the GPU.
		ENGCOM_API bool			PreRender ( rrCameraPass* cameraPass ) override;
		//	Render(const rrRenderParams* params) : Current pass
		ENGCOM_API bool			Render ( const rrRenderParams* params ) override;

		//	BeginRender() : Called before the render-loop executes.
		// Called once per frame.
		ENGCOM_API bool			BeginRender ( void ) override;
		//	EndRender() : Called after the render-loop executes.
		// Called once per frame.
		ENGCOM_API bool			EndRender ( void ) override;

	private:

		//	ERUpdateRenderList() : Updates the render list.
		void					ERUpdateRenderList ( std::vector<Element*>* renderList );

		//	ERUpdateRenderTarget() : Updates & resizes render target.
		// Returns true when there has been a change to the render target.
		bool					ERUpdateRenderTarget ( void );

		//	ERRenderElements() : Renders the elements to buffer.
		void					ERRenderElements ( std::vector<Element*>& const renderList, Rect& const scissorArea );

	private:

		// Rendering state
		gpu::RenderTarget	m_renderTarget;
		gpu::Texture		m_renderTargetTexture;
		Vector2i			m_renderTargetSize;

		UserInterface*		m_interface;
		RrFontTexture*		m_fontTexture;

		arModelData			m_modeldata;
		rrMeshBuffer		m_meshBuffer;

	};
}

#endif//ENGINE_COMMON_DUSK_UI_RENDERER_H_