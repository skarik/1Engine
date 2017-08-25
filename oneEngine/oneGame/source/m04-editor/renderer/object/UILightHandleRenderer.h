
#ifndef _EDITOR_UI_LIGHTHANDLE_RENDERER_H_
#define _EDITOR_UI_LIGHTHANDLE_RENDERER_H_

#include "renderer/object/CRenderableObject.h"

namespace M04
{
	class UILightHandle;
	class UILightHandleRenderer : public CRenderableObject
	{
	public:
		explicit	UILightHandleRenderer ( UILightHandle* );
		~UILightHandleRenderer( void );

		bool Render ( const char pass ) override;

	public:
		UILightHandle*	m_ui;
	};
}


#endif//_EDITOR_UI_DRAGHANDLE_RENDERER_H_