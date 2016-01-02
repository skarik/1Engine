#ifndef _M04_EDITOR_DEVELOPER_MENU_
#define _M04_EDITOR_DEVELOPER_MENU_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/CDuskGUIHandle.h"

class CCamera;
class CDuskGUI;
namespace Engine2D
{
	class TileMap;
	class Tileset;
}
namespace M04
{
	class MapInformation;
}

namespace M04
{
	class TileSelector;

	class DeveloperMenu : public CGameBehavior
	{
	public:
		explicit	DeveloperMenu ( void );
					~DeveloperMenu ( void );

		void		Update ( void ) override;

	protected:
		//		uiCreate () : create the dusk UI
		// create entirety of the dusk gui shit
		void		uiCreate ( void );

		//		uiStepMainPanel () : main panel update
		// handle inputs to the buttons on main panel
		void		uiStepMainPanel ( void );

	protected:
		CDuskGUI*	dusk;

		//=========================================//
		// Dusk handles

		Dusk::Handle	ui_main_game;
		Dusk::Handle	ui_main_test0;
		Dusk::Handle	ui_main_editor;
	};
}

#endif//_M04_EDITOR_DEVELOPER_MENU_
