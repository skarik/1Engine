
#ifndef _C_DUSK_GUI_DROPDOWN_LIST_
#define _C_DUSK_GUI_DROPDOWN_LIST_

#include "CDuskGUIButton.h"
#include <vector>
#include <string>

class CDuskGUIDropdownList : public CDuskGUIButton
{
public:
	CDuskGUIDropdownList ( void ) : CDuskGUIButton(), inDialogueMode( false ), homeRect(rect), selection(0), isReady(true) { ; };

	// Overridable update
	void Update ( void );
	void Render ( void );
	

	// Element for list
	struct ListElement_t {
		string str;
		int	   value;
	};

public:

	Rect homeRect;

	bool inDialogueMode;
	bool isReady;

	std::vector<ListElement_t> optionList;
	int  selection;
	int  oldSelection;
};

#endif