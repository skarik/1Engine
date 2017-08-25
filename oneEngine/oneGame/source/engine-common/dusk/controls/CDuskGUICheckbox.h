
#ifndef _C_DUSK_GUI_CHECKBOX_
#define _C_DUSK_GUI_CHECKBOX_

#include "CDuskGUIButton.h"

class CDuskGUICheckbox : public CDuskGUIButton
{
public:
	// Overridable update
	void Update ( void );
	void Render ( void );

public:
	bool boolValue;
	bool lastBoolValue;
};


#endif