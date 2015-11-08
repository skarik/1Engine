
#ifndef _C_DUSK_GUI_DRAGGABLE_PANEL_H_
#define _C_DUSK_GUI_DRAGGABLE_PANEL_H_

#include "CDuskGUIPanel.h"

class CDuskGUIDraggablePanel : public CDuskGUIPanel
{
public:
	CDuskGUIDraggablePanel() : CDuskGUIPanel(7) {}

	void Update ( void );
	void Render ( void );
	bool isDragging;
	//Vector2d vSourcePoint;
};

#endif//_C_DUSK_GUI_DRAGGABLE_PANEL_H_