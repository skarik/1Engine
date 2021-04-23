#ifndef M04_EDITORS_SEQUENCE_EDITOR_GRID_GIZMO_H_
#define M04_EDITORS_SEQUENCE_EDITOR_GRID_GIZMO_H_

#include "m04/eventide/Element.h"

namespace m04 {
namespace editor {
	class SequenceEditor;
}}

namespace m04 {
namespace editor {
namespace sequence {

	struct GridState
	{
		Real					gridSize = 32.0F;
		bool					snapX = true;
		bool					snapY = true;
	};

	class GridGizmo : public ui::eventide::Element
	{
	public:
		explicit				GridGizmo ( ui::eventide::UserInterface* ui, m04::editor::SequenceEditor* editor );

	protected:
								~GridGizmo ( void );

	public:
		virtual void			BuildMesh ( void ) override;
		virtual void			OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

	private:
		ui::eventide::Texture	m_texture;
		m04::editor::SequenceEditor*
								main_editor;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_GRID_GIZMO_H_