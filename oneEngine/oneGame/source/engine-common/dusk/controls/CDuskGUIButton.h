
#ifndef _C_DUSK_GUI_BUTTON_
#define _C_DUSK_GUI_BUTTON_

#include "CDuskGUIPanel.h"

class CDuskGUIButton : public CDuskGUIPanel
{
public:
	CDuskGUIButton() : CDuskGUIPanel(2) {}

	// Overridable update
	void Update ( void );
	void Render ( void );

public:
	// Button state
	bool	isPressed;

	bool	beginPress;
};

#endif