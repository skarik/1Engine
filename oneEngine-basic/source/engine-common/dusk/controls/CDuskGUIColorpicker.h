

#ifndef _C_DUSK_GUI_COLORPICKER_
#define _C_DUSK_GUI_COLORPICKER_

#include "CDuskGUIButton.h"
#include "core/math/Color.h"

class CDuskGUIColorpicker : public CDuskGUIButton
{
public:
	CDuskGUIColorpicker ( void ) : 
		CDuskGUIButton(), inDialogueMode(false),
		homeRect(rect), hue(-1.0f), useMode(0),
		colorDialogue(-1)
	{ ; }

	// Overridable update
	void Update ( void );
	void Render ( void );

	void SetColor ( Color& );

public:
	Color colorValue;
	Color lastColorValue;
	Color homeColorValue;

	bool inDialogueMode;

	Handle colorDialogue;

	Rect homeRect;

	ftype hue;
	ftype saturation;
	ftype lightness;

	int useMode;
};


#endif