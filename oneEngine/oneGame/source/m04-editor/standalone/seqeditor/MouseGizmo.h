#ifndef M04_EDITORS_SEQUENCE_EDITOR_MOUSE_GIZMO_H_
#define M04_EDITORS_SEQUENCE_EDITOR_MOUSE_GIZMO_H_

#include "m04/eventide/Element.h"

namespace m04 {
namespace editor {
namespace sequence {

	class MouseGizmo : public ui::eventide::Element
	{
	public:
		explicit				MouseGizmo ( ui::eventide::UserInterface* ui = NULL );

	protected:
								~MouseGizmo ( void );

	public:
		virtual void			BuildMesh ( void ) override;
		virtual void			OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

	private:
		ui::eventide::Texture	m_texture;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_MOUSE_GIZMO_H_