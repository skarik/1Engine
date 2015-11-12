
#ifndef _C_DUSK_GUI_PANEL_
#define _C_DUSK_GUI_PANEL_

#include "CDuskGUIElement.h"

class CDuskGUIPanel : public CDuskGUIElement
{
public:
	explicit CDuskGUIPanel ( const int ntype ) : CDuskGUIElement( ntype ) {}

	// Overridable update
	void Update ( void );
	void Render ( void );
};

#endif