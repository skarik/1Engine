
#ifndef _C_DUSK_GUI_FLOATFIELD_H_
#define _C_DUSK_GUI_FLOATFIELD_H_

#include "CDuskGUIPanel.h"

class CDuskGUIFloatfield : public CDuskGUIPanel
{
public:
	CDuskGUIFloatfield() : CDuskGUIPanel(16) {}

	// Overridable update
	void Update ( void );
	void Render ( void );

	float floatValue;
	float lastFloatValue;

	bool hasValidValue;
public:
	void GetFloatValue ( void );
};

#endif