#include "engine-common/dusk/controls/Panel.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::Panel::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);
}

void dusk::elements::Panel::Render ( UIRendererContext* uir )
{
	if (m_displayLabel)
	{
		uir->setFocus(dusk::kFocusStyleAutomatic);
		
		float textHeight = uir->getTextHeight(dusk::kTextFontTitle);
		uir->setColor(dusk::kColorStyleBackground);
		uir->drawRectangle(this, Rect(m_absoluteRect.pos, Vector2f(m_absoluteRect.size.x, textHeight + 10)));
		uir->drawRectangle(this, Rect(m_absoluteRect.pos + Vector2f(0.0F, textHeight + 10), m_absoluteRect.size - Vector2f(0.0F, textHeight + 10)));

		uir->setColor(dusk::kColorStyleLabel);
		uir->setTextSettings(TextStyleSettings{dusk::kTextFontTitle, dusk::kTextAlignLeft, dusk::kTextAlignTop});
		uir->drawText(this, m_absoluteRect.pos + Vector2f(5, 5), m_contents.c_str());
	}
	else
	{
		uir->setFocus(dusk::kFocusStyleAutomatic);
		uir->setColor(dusk::kColorStyleBackground);
		uir->drawRectangle(this, m_absoluteRect);
	}
}