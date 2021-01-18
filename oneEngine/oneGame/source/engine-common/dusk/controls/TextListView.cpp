#include "TextListView.h"

#include "core/input/CInput.h"
#include "core-ext/system/shell/Inputs.h"

#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"
#include "engine-common/dusk/controls/ScrollBar.h"

dusk::elements::TextListView::TextListView (void)
	: dusk::Element()
{
}

dusk::elements::TextListView::~TextListView (void)
{
	delete m_scrollBar;
}

void dusk::elements::TextListView::PostCreate ( void )
{
	m_scrollBar = m_interface->Add<dusk::elements::ScrollBar>(dusk::ElementCreationDescription(this, Rect()));
}

void dusk::elements::TextListView::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	if ( !m_isEnabled )
	{
		
	}
	else
	{
		if ( m_isMouseIn && m_wasDrawn )
		{
			// Update the hovered item
			for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
			{
				m_entries[menuIndex].hovered = false;
				m_entries[menuIndex].activated = false;
			}
			for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
			{
				if (m_entries[menuIndex].rect.Contains(stepinfo->mouse_position))
				{
					m_entries[menuIndex].hovered = true;
					break;
				}
			}

			// Update clicks on hovered items
			if ( core::Input::MouseDown(core::kMBLeft) )
			{
				for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
				{
					if (m_entries[menuIndex].hovered)
					{
						m_entries[menuIndex].selected = true;
						m_entries[menuIndex].activated = true;

						// Check double click time
						if (Time::currentTime - m_entries[menuIndex].lastClickTime < core::shell::GetDoubleClickInterval())
						{
							if (m_onDoubleClickEntry != nullptr)
							{
								m_onDoubleClickEntry(this, menuIndex);
							}
						}
						else
						{
							if (m_onClickEntry != nullptr)
							{
								m_onClickEntry(this, menuIndex);
							}
						}

						// Save time last clicked
						m_entries[menuIndex].lastClickTime = Time::currentTime;
					}
					else
					{
						m_entries[menuIndex].selected = false;
					}
				}
			}
		}
		else
		{
			for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
			{
				m_entries[menuIndex].hovered = false;
				m_entries[menuIndex].activated = false;
			}
		}
	}

	m_scrollBar->m_visible = (m_entries.size() * 16.0F) > m_absoluteRect.size.y; // TODO properly
	m_scrollBar->m_isEnabled = m_isEnabled && m_scrollBar->m_visible;
	m_scrollBar->m_localRect = Rect(Vector2f(m_absoluteRect.size.x - dusk::elements::ScrollBar::GetPreferredWidth(), 0.0F),
									Vector2f(dusk::elements::ScrollBar::GetPreferredWidth(), m_absoluteRect.size.y));
	m_scrollBar->m_scrollSize = m_entries.size() * 16.0F - m_absoluteRect.size.y; // TODO properly
	m_scrollBar->m_viewSize = m_absoluteRect.size.y;

	m_isActivated = false;
}
void dusk::elements::TextListView::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomaticNoHover);
	uir->setColor(dusk::kColorStyleBackground);
	uir->drawRectangle(this, m_absoluteRect);

	uir->setScissor(m_absoluteRect);
	float listItemsYOffset = (m_scrollBar != NULL && m_scrollBar->m_visible) ? -m_scrollBar->GetScroll() : 0.0F;
	for (uint menuIndex = 0; menuIndex < m_entries.size(); ++menuIndex)
	{
		Vector2f corner = m_absoluteRect.pos + Vector2f(5.0F, uir->getTextHeight(kTextFontButton) * menuIndex + listItemsYOffset);
		Rect bg (corner, Vector2f(uir->getTextWidth(kTextFontButton, m_entries[menuIndex].contents.c_str()), uir->getTextHeight(kTextFontButton)));

		m_isActivated = m_entries[menuIndex].activated;

		uir->setFocus(m_entries[menuIndex].hovered ? dusk::kFocusStyleHovered : dusk::kFocusStyleAutomaticNoHover);
		uir->setColor(m_entries[menuIndex].selected ? dusk::kColorStyleElementEmphasized : dusk::kColorStyleBackground, menuIndex + 1);
		uir->drawRectangle(this, bg);

		uir->setColor(dusk::kColorStyleLabel, menuIndex + 1);
		uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
		uir->drawText(this, m_absoluteRect.pos + Vector2f(5.0F, uir->getTextHeight(kTextFontButton) * (menuIndex + 0.5F) + listItemsYOffset), m_entries[menuIndex].contents.c_str());

		m_entries[menuIndex].rect = bg;
	}
}

void dusk::elements::TextListView::ClearMenuEntries ( void )
{
	m_entries.clear();
	m_scrollBar->SetScroll(0.0F);
}
void dusk::elements::TextListView::AddMenuEntry ( const char* menuEntry )
{
	m_entries.push_back(MenuEntry());
	m_entries.back().contents = menuEntry;
}