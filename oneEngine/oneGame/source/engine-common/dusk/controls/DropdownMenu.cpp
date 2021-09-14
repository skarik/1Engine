#include "engine-common/dusk/controls/DropdownMenu.h"

#include "core/input/CInput.h"

#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

// Padding used for the look of the dropdown menus
static const Vector2f	kPadding = {5.0F, 3.0F};

// List of all menus instantiated. Used for interaction between seperate dropdown menu elements.
static std::vector<dusk::elements::DropdownMenu*>
						g_DropdownMenus;

dusk::elements::DropdownMenu::DropdownMenu (void)
	: dusk::elements::Button()
{
	g_DropdownMenus.push_back(this);
}

dusk::elements::DropdownMenu::~DropdownMenu (void)
{
	g_DropdownMenus.erase(std::remove(g_DropdownMenus.begin(), g_DropdownMenus.end(), this), g_DropdownMenus.end());

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
		Real l_currentEntryHeight = 0.0F;
		for (size_t entryIndex = 0; entryIndex < m_entries.size(); ++entryIndex)
		{
			const MenuEntry& entry = m_entries[entryIndex];
			Button* button = m_dropdownMenu[entryIndex];

			switch (entry.type)
			{
			case MenuEntryType::kEntry:
				button->m_parent = this;
				button->m_localRect = Rect(
					Vector2f(0.0F, m_localRect.size.y + l_currentEntryHeight),
					Vector2f(m_menuWidth, m_entryHeight)
				);
				button->m_contents = entry.name;
				button->m_visible = m_open;
			
				l_currentEntryHeight += button->m_localRect.size.y;
				break;

			case MenuEntryType::kSpacer:
				button->m_parent = this;
				button->m_localRect = Rect(
					Vector2f(0.0F, m_localRect.size.y + l_currentEntryHeight),
					Vector2f(m_menuWidth, m_entryHeight * 0.5F)
				);
				button->m_contents = entry.name;
				button->m_visible = m_open;
				
				button->m_canFocus = false;
				button->m_isEnabled = false;

				l_currentEntryHeight += button->m_localRect.size.y;
				break;
			}
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

			if (entry.type == MenuEntryType::kEntry
				&& button->m_isActivated)
			{
				if (entry.hide_menu)
				{
					HideMenu();
				}
				if (entry.action != NULL)
				{
					entry.action();
				}
			}
		}

		// If there was a click, ensure that we're inside the menu
		if (core::Input::MouseDown(core::kMBLeft, stepinfo->input_index))
		{
			bool l_containsClick = m_isMouseIn;
			if (!l_containsClick)
			{
				for (Button* button : m_dropdownMenu)
				{
					if (button->m_isMouseIn)
					{
						l_containsClick = true;
						break;
					}
				}
			}

			if (!l_containsClick)
			{
				HideMenu();
			}
		}

		// If the user hit escape, assume we're trying to close the current menu.
		// TODO: handle submenus
		if (core::Input::Keydown(VK_ESCAPE, stepinfo->input_index))
		{
			HideMenu();
		}

		// If the menu is suddenly disabled, we need to also close
		if (!m_isEnabled)
		{
			HideMenu();
		}
	}
	else
	{
		// We're not in and not open? We want to check our neighbors w/ the same parent
		if (m_isMouseIn)
		{
			for (DropdownMenu* menu : g_DropdownMenus)
			{
				if (menu->m_parent == m_parent && menu != this
					&& menu->m_open)
				{
					// Move to this menu naturally instead.
					menu->HideMenu();
					this->ShowMenu();
					break;
				}
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

	// Find all menus with the same parent and hide them.
	for (DropdownMenu* menu : g_DropdownMenus)
	{
		if (menu->m_parent == m_parent && menu != this)
		{
			menu->HideMenu();
		}
	}
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
}