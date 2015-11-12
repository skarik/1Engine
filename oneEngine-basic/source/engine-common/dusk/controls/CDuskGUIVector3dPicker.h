// 3D vector picker is a pop-out dialogue for typing in values
// It also has a normalize button for all your wonderful needs.
// If it is NOT popped out, the float fields are organized horizontally.
// The fields can be tabbed to as they are actual child controls.

#ifndef _C_DUSK_GUI_VECTOR_3D_PICKER_H_
#define _C_DUSK_GUI_VECTOR_3D_PICKER_H_

#include "CDuskGUIButton.h"
#include "core/math/Vector3d.h"

class CDuskGUIVector3dPicker : public CDuskGUIButton
{
public:
	CDuskGUIVector3dPicker ( void );
	~CDuskGUIVector3dPicker ( void );

	// Overridable update
	void Update ( void );
	void Render ( void );

	void SetVector ( Vector3d& );

public:
	Vector3d colorValue;
	Vector3d lastColorValue;

	bool inDialogueMode;

	Rect homeRect;

	// Float field handles
	Handle hVecX;
	Handle hVecY;
	Handle hVecZ;
};

#endif//_C_DUSK_GUI_VECTOR_3D_PICKER_H_