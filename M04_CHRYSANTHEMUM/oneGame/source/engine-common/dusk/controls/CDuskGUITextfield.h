
#ifndef _C_DUSK_GUI_TEXTFIELD_H_
#define _C_DUSK_GUI_TEXTFIELD_H_

#include "CDuskGUIPanel.h"
#include <string>
using std::string;

class CDuskGUITextfield : public CDuskGUIPanel
{
public:
	CDuskGUITextfield() : CDuskGUIPanel(13) {}

	// Overridable update
	void Update ( void );
	void Render ( void );

	string textValue;
	string lastTextValue;
public:
	void GetStringValue ( void );
};

#endif