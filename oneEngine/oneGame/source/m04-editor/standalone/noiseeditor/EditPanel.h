#ifndef M04_EDITORS_NOISE_EDITOR_EDIT_PANEL_H_
#define M04_EDITORS_NOISE_EDITOR_EDIT_PANEL_H_

#include "engine-common/dusk/Dusk.h"

namespace dusk {
namespace elements {
	class Button;
}}

namespace m04 {
namespace editor {
	class NoiseEditor;
}}

namespace m04 {
namespace editor {
namespace noise {

	class EditPanel
	{
	public:
		explicit				EditPanel (dusk::UserInterface* ui, m04::editor::NoiseEditor* editor);
								~EditPanel ( void );

		void					Update ( void );

	private:

		dusk::UserInterface*
							dusk_interface;

		m04::editor::NoiseEditor*
							main_editor;

	};

}}}

#endif//M04_EDITORS_NOISE_EDITOR_EDIT_PANEL_H_