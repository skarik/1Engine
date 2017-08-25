
// A tabview is a horizontal listview in function.
// However, as each tab has its own panel, you can parent elements to a tab.

#ifndef _C_DUSK_GUI_TABVIEW_H_
#define _C_DUSK_GUI_TABVIEW_H_

#include "CDuskGUIPanel.h"

class CDuskGUIListview : public CDuskGUIPanel
{
public:
	CDuskGUIListview ( void ) : selection(-1), CDuskGUIPanel(), field_height(0.04f) { ; };
	~CDuskGUITabview ( void );

	// Overridable update
	void Update ( void );
	void Render ( void );

	// Element for list
	struct ListElement_t {
		string str;
		int	   value;
		CDuskGUI::Handle panel;
	};

public:
	vector<ListElement_t> optionList;
	int  selection;
	int  currentmouseover;
	Real field_height;
};

#endif//_C_DUSK_GUI_TABVIEW_H_