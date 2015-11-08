// MaterialField
// A textfield that attempts to load a material when a valid file is found.
// Given a pointer to a material, it will modify the material to be a copy of whatever
// material was selected.
// Eventually, will have a button to create a MaterialPicker dialogue

#ifndef _C_DUSK_GUI_MATERIAL_FIELD_H_
#define _C_DUSK_GUI_MATERIAL_FIELD_H_

#include "CDuskGUITextfield.h"

class glMaterial;

class CDuskGUIMaterialField : public CDuskGUITextfield
{
public:
	CDuskGUIMaterialField ( void ) : CDuskGUITextfield(), target(NULL), hasValidValue(false) {;}; 

	// Overridable update
	void Update ( void );
	void Render ( void );

	bool hasValidValue;
	glMaterial* target;
};

#endif//_C_DUSK_GUI_MATERIAL_FIELD_H_