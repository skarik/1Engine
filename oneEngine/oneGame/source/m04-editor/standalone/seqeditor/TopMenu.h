#ifndef M04_EDITORS_SEQUENCE_EDITOR_TOP_MENU_H_
#define M04_EDITORS_SEQUENCE_EDITOR_TOP_MENU_H_

#include "engine-common/dusk/Dusk.h"

namespace m04 {
namespace editor {
namespace sequence {

	class TopMenu
	{
	public:
		explicit				TopMenu (dusk::UserInterface* ui);
								~TopMenu ( void );

	private:

		dusk::UserInterface*
							dusk_interface;

	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_TOP_MENU_H_