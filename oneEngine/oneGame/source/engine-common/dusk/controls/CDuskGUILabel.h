
#ifndef _C_DUSK_GUI_LABEL_H_
#define _C_DUSK_GUI_LABEL_H_

#include "CDuskGUIPanel.h"

class CDuskGUILabel : public CDuskGUIPanel
{
public:
	CDuskGUILabel ( void ) : CDuskGUIPanel(1) { canHaveFocus = false;};

	// Overridable update
	void Update ( void );
	void Render ( void );

};

#endif