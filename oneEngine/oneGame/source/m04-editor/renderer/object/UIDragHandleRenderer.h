
#ifndef _EDITOR_UI_DRAGHANDLE_RENDERER_H_
#define _EDITOR_UI_DRAGHANDLE_RENDERER_H_

#include "renderer/object/CRenderableObject.h"

namespace M04
{
	class UIDragHandle;
	class UIDragHandleRenderer : public CRenderableObject
	{
	public:
		explicit	UIDragHandleRenderer ( UIDragHandle* );
					~UIDragHandleRenderer( void );

		bool Render ( const char pass ) override;

	public:
		UIDragHandle*	m_ui;
	};
}


#endif//_EDITOR_UI_DRAGHANDLE_RENDERER_H_