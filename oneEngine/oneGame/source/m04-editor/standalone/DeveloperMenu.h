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

		dusk::Element*	ui_paletted_3d_test;
		
		dusk::Element*	ui_seq_editor;
		dusk::Element*	ui_noise_editor;

		dusk::Element*	ui_model_editor;

		dusk::Element*	ui_material_editor;
		dusk::Element*	ui_material_browser;
		dusk::Element*	ui_level_builder;
	};
}

#endif//_M04_EDITOR_DEVELOPER_MENU_
