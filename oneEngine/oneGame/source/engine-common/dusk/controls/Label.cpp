#include "engine-common/dusk/controls/Label.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::Label::Update ( void )
{
	Element::Update();
}

void dusk::elements::Label::Render ( UIRendererContext* uir )
{
	uir->setColor(dusk::kColorStyleLabel);
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontTitle, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
	uir->drawText(this, m_absoluteRect.pos, m_contents.c_str());
}