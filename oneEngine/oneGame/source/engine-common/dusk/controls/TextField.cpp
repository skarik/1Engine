#include "TextField.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::TextField::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	if ( !m_isEnabled )
	{
	}
	else
	{
		if ( m_isFocused )
		{
			const auto& inputString = core::Input::FrameInputString( stepinfo->input_index );
			for (const auto& input : inputString)
			{
				if ( input && isprint(input) )
				{
					m_contents += input;
				}
				if ( input == core::kVkBackspace )
				{
					m_contents = m_contents.substr(0, m_contents.length() - 1);
				}
			}
		}

		if ( m_isMouseIn && m_wasDrawn )
		{
			// Mouse controls
			if ( core::Input::MouseDown( core::kMBLeft, stepinfo->input_index ) )
			{
			}
		}
		else
		{
		}
	}

	//m_isActivated = isPressed;
}
void dusk::elements::TextField::Render ( UIRendererContext* uir )
{
	if (m_drawHeight <= 0.0F)
	{
		m_drawHeight = (Real)math::round(uir->getTextHeight(kTextFontButton) * 1.5F + 5.0F);
	}
	m_absoluteRect.pos.y += (Real)math::round((m_absoluteRect.size.y - m_drawHeight) * 0.5F);
	m_absoluteRect.size.y = m_drawHeight;

	uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleBackground, 0);
	uir->drawRectangle(this, m_absoluteRect);
	uir->setFocus(m_isFocused ? dusk::kFocusStyleFocused : dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleElementEmphasized, 1);
	uir->drawBorder(this, m_absoluteRect);

	uir->setColor(dusk::kColorStyleLabel);
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
	uir->drawText(this, m_absoluteRect.pos + Vector2f(5.0F, m_absoluteRect.size.y * 0.5F - 1.0F), m_contents.c_str());
}