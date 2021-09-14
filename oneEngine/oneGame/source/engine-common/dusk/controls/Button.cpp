#include "engine-common/dusk/controls/Button.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::Button::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	isPressed = false;

	if ( !m_isEnabled )
	{
		beginPress = false;
		isPressed = false;
	}
	else
	{
		if ( m_isMouseIn && m_wasDrawn )
		{
			// Mouse controls
			if ( core::Input::MouseDown(core::kMBLeft, stepinfo->input_index) )
			{
				beginPress = true;
			}
			else if ( beginPress )
			{
				if ( core::Input::MouseUp(core::kMBLeft, stepinfo->input_index) )
				{
					beginPress = false;
					isPressed = true;
				}
			}
		}
		else
		{
			beginPress = false;

			// Keyboard controls
			if ( m_isFocused )
			{
				if ( core::Input::Keydown( core::kVkReturn, stepinfo->input_index ) )
				{
					isPressed = true;
				}
			}
		}
	}

	m_isActivated = isPressed;
}
void dusk::elements::Button::Render ( UIRendererContext* uir )
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

	uir->setColor(dusk::kColorStyleLabel);
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
	uir->drawText(this, m_absoluteRect.pos + Vector2f(5.0F, m_absoluteRect.size.y * 0.5F), m_contents.c_str());

	m_isMouseIn = mouseInPrevious;
}