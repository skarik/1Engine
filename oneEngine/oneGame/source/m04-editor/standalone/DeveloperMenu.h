#ifndef _M04_EDITOR_DEVELOPER_MENU_
#define _M04_EDITOR_DEVELOPER_MENU_

#include "engine/behavior/CGameBehavior.h"

class RrCamera;
namespace Engine2D
{
	class TileMap;
	class Tileset;
}
namespace M04
{
	class MapInformation;
}
namespace dusk
{
	class UserInterface;
	class Element;
}

namespace M04
{
	class TileSelector;

	class DeveloperMenu : public CGameBehavior
	{
	public:
		explicit			DeveloperMenu ( void );
							~DeveloperMenu ( void );

		void				Update ( void ) override;

	protected:
		//		uiCreate () : create the dusk UI
		// create entirety of the dusk gui shit
		void				uiCreate ( void );

		//		uiStepMainPanel () : main panel update
		// handle inputs to the buttons on main panel
		void				uiStepMainPanel ( void );

	protected:
		dusk::UserInterface*
						dusk;

		//=========================================//
		// Dusk handles

		dusk::Element*	ui_seq_editor;
		dusk::Element*	ui_cts_editor;

		dusk::Element*	ui_main_game;
		dusk::Element*	ui_main_testg;
		dusk::Element*	ui_main_test0;
		dusk::Element*	ui_main_editor;
	};
}

#endif//_M04_EDITOR_DEVELOPER_MENU_
