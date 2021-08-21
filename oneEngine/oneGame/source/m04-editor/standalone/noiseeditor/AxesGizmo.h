#ifndef M04_EDITOR_NOISE_EDITOR_AXES_GIZMO_H_
#define M04_EDITOR_NOISE_EDITOR_AXES_GIZMO_H_

#include "m04/eventide/Element.h"

namespace m04 {
namespace editor {
	class NoiseEditor;
}}

namespace m04 {
namespace editor {
namespace noise {

	struct AxesState
	{
		Real					gridSize = 32.0F;
		bool					snapX = true;
		bool					snapY = true;
	};

	class AxesGizmo : public ui::eventide::Element
	{
	public:
		explicit				AxesGizmo ( ui::eventide::UserInterface* ui );

	protected:
								~AxesGizmo ( void );

	public:
		virtual void			BuildMesh ( void ) override;
		virtual void			OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

	private:
		//ui::eventide::Texture	m_texture;
		//m04::editor::NoiseEditor*
		//						main_editor;
	};

}}}

#endif//M04_EDITOR_NOISE_EDITOR_AXES_GIZMO_H_