
#ifndef _C_DUSK_GUI_SLIDER_H_
#define _C_DUSK_GUI_SLIDER_H_

#include "CDuskGUIPanel.h"
#include "core/math/random/RangeValue.h"

class CDuskGUISlider : public CDuskGUIPanel
{
public:
	CDuskGUISlider ( void );

	// Overridable update
	void Update ( void );
	void Render ( void );

	Real divs;
	RangeValue<Real> value;
	RangeValue<Real> prevValue;

	bool isDragging;
};

#endif//_C_DUSK_GUI_SLIDER_H_