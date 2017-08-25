// These specialized dropdown lists populate themselves automatically.
// Based on the given filter, they will grab relevant values.
// Their selection choices correspond to GameBehavior ID's.

#ifndef _C_DUSK_GUI_DROPDOWN_LIST_GAMEOBJECT_H_
#define _C_DUSK_GUI_DROPDOWN_LIST_GAMEOBJECT_H_

#include "CDuskGUIDropdownList.h"

class CDuskGUIDropdownList_GO : public CDuskGUIDropdownList
{

public:
	CDuskGUIDropdownList_GO ( void ) : CDuskGUIDropdownList() { ; };

	void Update ( void );

	void RepopulateList ( void );


	string typenameFilter;
};

#endif//_C_DUSK_GUI_DROPDOWN_LIST_GAMEOBJECT_H_