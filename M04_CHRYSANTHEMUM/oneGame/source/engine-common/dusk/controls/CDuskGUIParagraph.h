
#ifndef _C_DUSK_GUI_PARAGRAPH_H_
#define _C_DUSK_GUI_PARAGRAPH_H_

#include "CDuskGUIPanel.h"

class CDuskGUIParagraph : public CDuskGUIPanel
{
public:
	CDuskGUIParagraph ( void ) : CDuskGUIPanel(5) { canHaveFocus=false; };

	// Overridable update
	void Update ( void );
	void Render ( void );

};

#endif