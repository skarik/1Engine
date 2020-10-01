#include "UserInterface.h"

#include "core/debug/Console.h"
#include "../eventide/Element.h"

static ui::eventide::UserInterface* l_ActiveManager = NULL;

ui::eventide::UserInterface* ui::eventide::UserInterface::Get ( void )
{
	if (l_ActiveManager == NULL)
	{
		l_ActiveManager = new ui::eventide::UserInterface(NULL, NULL);
	}
	return l_ActiveManager;
}

void ui::eventide::UserInterface::ReleaseActive ( void )
{
	delete l_ActiveManager;
	l_ActiveManager = NULL;
}


ui::eventide::UserInterface::UserInterface ( dusk::UserInterface* duskUI, dawn::UserInterface* dawnUI )
	: m_duskUI(duskUI)
	, m_dawnUI(dawnUI)
{
	;
}

ui::eventide::UserInterface::~UserInterface ( void )
{
	m_shuttingDown = true;

	// Free all allocated elements now
	for (ui::eventide::Element* element : m_elements)
	{
		delete element;
	}
	m_elements.clear();
}

void ui::eventide::UserInterface::AddElement ( ui::eventide::Element* element )
{
	if (!m_shuttingDown)
	{
		ARCORE_ASSERT(std::find(m_elements.begin(), m_elements.end(), element) == m_elements.end());
		m_elements.push_back(element);

		m_elementsDirty = true; // Mark elements dirty - we need a new update order
	}
}
void ui::eventide::UserInterface::RemoveElement ( ui::eventide::Element* element )
{
	if (!m_shuttingDown)
	{
		auto element_iter = std::find(m_elements.begin(), m_elements.end(), element);
		ARCORE_ASSERT(element_iter != m_elements.end());
		m_elements.erase(element_iter);

		// Check that we're not orphaning anyone here, and if we are, properly orphan them.
		for (ui::eventide::Element* element_check : m_elements)
		{
			if (element_check != NULL && element_check->GetParent() == element)
			{
				element_check->SetParent(NULL);
				debug::Console->PrintWarning("Orphaned eventide::Element (%p)\n.", element_check);
			}
		}

		m_elementsDirty = true; // Mark elements dirty - we need a new update order
	}
}

void ui::eventide::UserInterface::Update ( void )
{
	// Sort elements if there's been a change:
	if (m_elementsDirty)
	{
		std::vector<ui::eventide::Element*> new_update_list;

		auto AddNewElement = [&](ui::eventide::Element* current_element)
		{
			std::list<ui::eventide::Element*> process_list;
			process_list.push_back(current_element);

			while (!process_list.empty())
			{
				Element* element = process_list.front();
				process_list.pop_front();

				if (element->GetParent() == NULL)
				{
					new_update_list.push_back(element);
				}
				else
				{
					auto parent_element_itr = std::find(new_update_list.begin(), new_update_list.end(), element->GetParent());
					if (parent_element_itr == new_update_list.end()) // As items are added, this branch will be hit less and less, since the previous sorting speeds this up.
					{
						// Add parent to be the next item looked at
						process_list.push_front(element);
						process_list.push_front((ui::eventide::Element*)element->GetParent());
						continue;
					}
					else
					{
						// Now that we have the parent, just insert after it
						new_update_list.insert(++parent_element_itr, element);
					}
				}
			}
		};

		// go to parent, add parent, then add all the children
		for (ui::eventide::Element* element : m_elements)
		{
			AddNewElement(element);
		}

		ARCORE_ASSERT(m_elements.size() == new_update_list.size());

		// We now have a new update list!
		m_elements = new_update_list;
		m_elementsDirty = false;
	}

	// Update all transforms (perhaps this should be late-update?):

	// Update mouse clickity clack:
}

