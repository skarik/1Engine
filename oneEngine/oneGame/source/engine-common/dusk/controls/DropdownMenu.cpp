#include "engine-common/dusk/controls/DropdownMenu.h"

#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

static const Vector2f kPadding = {5.0F, 3.0F};

dusk::elements::DropdownMenu::~DropdownMenu (void)
{
	for (Button* button : m_dropdownMenu)
	{
		delete button;
	}
	m_dropdownMenu.clear();
}


void dusk::elements::DropdownMenu::Update ( const UIStepInfo* stepinfo )
{
	if (!m_sizeNeedsUpdate && m_localRect.size.x < FLOAT_PRECISION)
	{
		m_localRect.size.x = m_toplevelWidth;
		m_localRect.size.y = m_entryHeight;
	}

	if (m_elementsNeedUpdate && !m_sizeNeedsUpdate)
	{
		m_elementsNeedUpdate = false;

		// Resize the element array
		while (m_dropdownMenu.size() < m_entries.size())
		{
			m_dropdownMenu.push_back(m_interface->Add<Button>(dusk::ElementCreationDescription(this, Rect())));
		}
		for (size_t entryIndex = m_entries.size(); entryIndex < m_dropdownMenu.size(); ++entryIndex)
		{
			delete m_dropdownMenu[entryIndex];
		}
		m_dropdownMenu.resize(m_entries.size());

		// Apply changes to the entries
		for (size_t entryIndex = 0; entryIndex < m_entries.size(); ++entryIndex)
		{
			const MenuEntry& entry = m_entries[entryIndex];
			Button* button = m_dropdownMenu[entryIndex];

			button->m_parent = this;
			button->m_localRect = Rect(
				Vector2f(0.0F, m_localRect.size.y + m_entryHeight * entryIndex),
				Vector2f(m_menuWidth, m_entryHeight)
			);
			button->m_contents = entry.name;
			button->m_visible = m_open;
		}
	}

	Button::Update(stepinfo);

	// Check if should bring up menu or not
	if (m_isActivated)
	{
		m_open = !m_open;
		if (m_open)
		{
			ShowMenu();
		}
		else
		{
			HideMenu();
		}
	}

	// Check state of buttons
	if (m_open)
	{
		for (size_t entryIndex = 0; entryIndex < m_entries.size(); ++entryIndex)
		{
			const MenuEntry& entry = m_entries[entryIndex];
			Button* button = m_dropdownMenu[entryIndex];

			if (button->m_isActivated)
			{
				if (entry.hide_menu)
				{
					HideMenu();
				}
				entry.action();
			}
		}
	}
}

void dusk::elements::DropdownMenu::ShowMenu ( void )
{
	for (Button* button : m_dropdownMenu)
	{
		button->m_visible = true;
	}
	m_open = true;
}

void dusk::elements::DropdownMenu::HideMenu ( void )
{
	for (Button* button : m_dropdownMenu)
	{
		button->m_visible = false;
	}
	m_open = false;
}

void dusk::elements::DropdownMenu::Render ( UIRendererContext* uir )
{
	/*if (m_displayLabel)
	{
		uir->setFocus(dusk::kFocusStyleAutomaticNoHover);

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
		uir->setFocus(dusk::kFocusStyleAutomaticNoHover);
		uir->setColor(dusk::kColorStyleBackground);
		uir->drawRectangle(this, m_absoluteRect);
	}*/

	Button::Render(uir);

	if (m_sizeNeedsUpdate)
	{
		m_sizeNeedsUpdate = false;

		// Update top level
		m_toplevelWidth = uir->getTextWidth(dusk::kTextFontButton, m_contents.c_str());

		// Get largest entry
		m_menuWidth = 0.0F;
		for (MenuEntry& entry : m_entries)
		{
			m_menuWidth = std::max(m_menuWidth, uir->getTextWidth(dusk::kTextFontButton, entry.name.c_str()));
		}

		// Get text height
		m_entryHeight = uir->getTextHeight(dusk::kTextFontButton);

		// Apply padding
		m_toplevelWidth += kPadding.x * 2;
		m_menuWidth += kPadding.x * 2;
		m_entryHeight += kPadding.y * 2;
	}

	/*uir->setFocus(dusk::kFocusStyleAutomatic);
	uir->setColor(dusk::kColorStyleElement);
	uir->drawRectangle(this, m_absoluteRect);

	uir->setColor(dusk::kColorStyleLabel);
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignMiddle});
	uir->drawText(this, m_absoluteRect.pos + Vector2f(5.0F, m_absoluteRect.size.y * 0.5F), m_contents.c_str());

	if (m_open)
	{
		const float kEntryHeight = uir->getTextHeight(dusk::kTextFontButton) + kPadding.y * 2;

		for (size_t entryIndex = 0; entryIndex < m_entries.size(); ++entryIndex)
		{
			const Rect entryRect (
				m_absoluteRect.pos + Vector2f(0.0F, m_absoluteRect.size.y + entryIndex * kEntryHeight),
				Vector2f(m_menuWidth, kEntryHeight));

			uir->setFocus(dusk::kFocu);
			uir->setColor(dusk::kColorStyleElement);
			uir->drawRectangle(this, m_absoluteRect);
		}
	}*/
}