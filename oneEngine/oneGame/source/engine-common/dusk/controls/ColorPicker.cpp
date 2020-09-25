#include "ColorPicker.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::ColorPicker::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);
}
void dusk::elements::ColorPicker::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);
}