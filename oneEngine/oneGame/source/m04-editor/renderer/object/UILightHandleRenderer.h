#ifndef EDITOR_UI_LIGHTHANDLE_RENDERER_H_
#define EDITOR_UI_LIGHTHANDLE_RENDERER_H_

#include "renderer/object/CStreamedRenderable3D.h"

namespace M04
{
	class UILightHandle;
	class UILightHandleRenderer : public CStreamedRenderable3D
	{
	public:
		explicit	UILightHandleRenderer ( UILightHandle* );
		~UILightHandleRenderer( void );

		bool PreRender ( void ) override;

	public:
		UILightHandle*	m_ui;
	};
}


#endif//EDITOR_UI_DRAGHANDLE_RENDERER_H_