// These specialized dropdown lists populate themselves automatically.
// Based on the given filter, they will grab relevant values.
// Their selection choices correspond to GameBehavior ID's.

#ifndef _C_DUSK_GUI_DROPDOWN_LIST_LOGICOBJECT_H_
#define _C_DUSK_GUI_DROPDOWN_LIST_LOGICOBJECT_H_

#include "CDuskGUIDropdownList.h"

class CDuskGUIDropdownList_LO : public CDuskGUIDropdownList
{

public:
	CDuskGUIDropdownList_LO ( void ) : CDuskGUIDropdownList() { ; };

	void Update ( void );

	void RepopulateList ( void );


	string typenameFilter;
};

#endif//_C_DUSK_GUI_DROPDOWN_LIST_GAMEOBJECT_H_