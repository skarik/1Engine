#ifndef EDITOR_UI_DRAGHANDLE_RENDERER_H_
#define EDITOR_UI_DRAGHANDLE_RENDERER_H_

#include "renderer/object/CStreamedRenderable3D.h"

namespace M04
{
	class UIDragHandle;
	class UIDragHandleRenderer : public CStreamedRenderable3D
	{
	public:
		explicit				UIDragHandleRenderer ( UIDragHandle* );
								~UIDragHandleRenderer( void );

		bool					EndRender ( void ) override;

	public:
		UIDragHandle*		m_ui;
	};
}


#endif//EDITOR_UI_DRAGHANDLE_RENDERER_H_