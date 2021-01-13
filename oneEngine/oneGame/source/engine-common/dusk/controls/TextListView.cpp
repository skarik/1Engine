#include "TextListView.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

dusk::elements::TextListView::TextListView (void)
	: dusk::Element()
{
}

dusk::elements::TextListView::~TextListView (void)
{
}

void dusk::elements::TextListView::Update ( const UIStepInfo* stepinfo )
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

			// Update the hovered item
			for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
			{
				m_entries[menuIndex].hovered = false;
			}
			for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
			{
				if (m_entries[menuIndex].rect.Contains(stepinfo->mouse_position))
				{
					m_entries[menuIndex].hovered = true;
					break;
				}
			}
		}
		else
		{
			for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
			{
				m_entries[menuIndex].hovered = false;
			}
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
void dusk::elements::TextListView::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomaticNoHover);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);

	for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
	{
		Vector2f corner = m_absoluteRect.pos + Vector2f(5.0F, uir->getTextHeight(kTextFontButton) * menuIndex);
		Rect bg (corner, Vector2f(uir->getTextWidth(kTextFontButton, m_entries[menuIndex].contents.c_str()), uir->getTextHeight(kTextFontButton)));

		uir->setFocus(m_entries[menuIndex].hovered ? dusk::kFocusStyleHovered : dusk::kFocusStyleAutomaticNoHover);
		uir->setColor(dusk::kColorStyleBackground, menuIndex + 1);
		uir->drawRectangle(this, bg);

		uir->setColor(dusk::kColorStyleLabel, menuIndex + 1);
		uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
		uir->drawText(this, m_absoluteRect.pos + Vector2f(5.0F, uir->getTextHeight(kTextFontButton) * (menuIndex + 0.5F)), m_entries[menuIndex].contents.c_str());

		m_entries[menuIndex].rect = bg;
	}
}

void dusk::elements::TextListView::ClearMenuEntries ( void )
{
	m_entries.clear();
}
void dusk::elements::TextListView::AddMenuEntry ( const char* menuEntry )
{
	//m_entries.push_back({menuEntry, Rect()});
	m_entries.push_back(MenuEntry());
	m_entries.back().contents = menuEntry;
}