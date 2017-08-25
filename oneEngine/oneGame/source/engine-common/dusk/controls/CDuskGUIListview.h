
// CDuskGUIListview
//  Elements in a listview are listed vertically.
//  If the contents goes beyond the range of the control, a scrollbar is added.
//  The currently selected element can be set and get via external functions.

#ifndef _C_DUSK_GUI_LISTVIEW_H_
#define _C_DUSK_GUI_LISTVIEW_H_

#include "CDuskGUIPanel.h"

class CDuskGUIListview : public CDuskGUIPanel
{
public:
	CDuskGUIListview ( void ) : selection(-1), CDuskGUIPanel(19), field_height(0.04f) { ; };

	// Overridable update
	void Update ( void );
	void Render ( void );

	// Element for list
	struct ListElement_t {
		string str;
		int	   value;
	};

public:
	std::vector<ListElement_t> optionList;
	int  selection;
	int  currentmouseover;
	Real field_height;
};

#endif//_C_DUSK_GUI_LISTVIEW_H_