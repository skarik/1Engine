#include "engine-common/dusk/controls/Checkbox.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::Checkbox::Update ( const UIStepInfo* stepinfo )
{
	Button::Update(stepinfo);

	if (m_isActivated)
	{
		m_value = !m_value;
		if (m_onValueChange != nullptr)
		{
			m_onValueChange(m_value);
		}
	}
}
void dusk::elements::Checkbox::Render ( UIRendererContext* uir )
{
	// If not enabled, temporarily disable mouse-in
	bool mouseInPrevious = m_isMouseIn;
	if (!m_isEnabled)
	{
		m_isMouseIn = false;
	}

	uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(m_emphasizeVisuals ? dusk::kColorStyleElementEmphasized : dusk::kColorStyleElement);
	uir->drawRectangle(this, m_absoluteRect);

	if (m_value)
	{
		uir->setColor(dusk::kColorStyleShapeNormal);

		const float kSubrectMargins = 4.0F;
		Rect subrect = m_absoluteRect;
		subrect.pos.x += kSubrectMargins;
		subrect.pos.y += kSubrectMargins;
		subrect.size.x -= kSubrectMargins;
		subrect.size.y -= kSubrectMargins;
		uir->drawRectangle(this, subrect);
	}

	m_isMouseIn = mouseInPrevious;
}