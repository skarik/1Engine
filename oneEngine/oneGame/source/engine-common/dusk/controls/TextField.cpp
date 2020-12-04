#include "TextField.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::TextField::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	//isPressed = false;

	if ( !m_isEnabled )
	{
		//beginPress = false;
		//isPressed = false;
	}
	else
	{
		if ( m_isMouseIn && m_wasDrawn )
		{
			// Mouse controls
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				//beginPress = true;
			}
			/*else if ( beginPress )
			{
				if ( CInput::MouseUp(CInput::MBLeft) )
				{
					beginPress = false;
					isPressed = true;
				}
			}*/
		}
		else
		{
			/*beginPress = false;

			// Keyboard controls
			if ( m_isFocused )
			{
				if ( CInput::Keydown( Keys.Return ) )
				{
					isPressed = true;
				}
			}*/
		}
	}

	//m_isActivated = isPressed;
}
void dusk::elements::TextField::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);

	uir->setColor(dusk::kColorStyleLabel);
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
	uir->drawText(this, m_absoluteRect.pos + Vector2f(5.0F, (m_absoluteRect.size.y - uir->getTextHeight(kTextFontButton)) * 0.5F), m_contents.c_str());
}