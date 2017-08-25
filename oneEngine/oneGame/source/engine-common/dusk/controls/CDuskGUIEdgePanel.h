
#ifndef _C_DUSK_GUI_EDGE_PANEL_H_
#define _C_DUSK_GUI_EDGE_PANEL_H_

#include "CDuskGUIPanel.h"

class CDuskGUIEdgePanel : public CDuskGUIPanel
{
public:
	CDuskGUIEdgePanel() : CDuskGUIPanel(7) {}

	void Update ( void );
	void Render ( void );

	Rect actual_rect;
};

#endif//_C_DUSK_GUI_EDGE_PANEL_H_