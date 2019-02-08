#include "UI.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "engine-common/dusk/Element.h"
#include "engine-common/dusk/UIRenderer.h"

//	AddInitialize(element, desc) : Initializes the element with the given description.
// Used after an element is instantiated, applies the given parameters.
Dusk::Element* Dusk::UserInterface::AddInitialize ( Element* element, ElementCreationDescription& const desc )
{
	element->m_interface = this; // Take ownership of this element

	element->m_localRect	= desc.localRect;
	//element->m_visible		= desc.visible;

	// Find the input parent within the given list
	size_t parentIndex = 0xFFFFFFFF;
	for (size_t i = 0; i < m_elements.size(); ++i)
	{
		if (m_elements[i] == desc.parent) {
			parentIndex = i;
		}
	}
	if (parentIndex != 0xFFFFFFFF)
	{
		element->m_parent = desc.parent;
		element->m_parentHandle = Dusk::Handle(parentIndex, this);
	}
	else
	{
		throw core::InvalidReferenceException();
	}

	// Update index
	element->m_index = m_elements.size();

	return element;
}

//	AddInitializeCheckValid(element) : Checks the given element has the correct index.
// To be used as part of an assertion check.
bool Dusk::UserInterface::AddInitializeCheckValid ( Element* element )
{
	return m_elements[element->m_index] == element;
}

Dusk::UserInterface::UserInterface ( void )
	: CGameBehavior()
{
	m_renderer = new UIRenderer(this);

	// Handle init
	m_currentElement = kElementHandleInvalid;
	m_currentDialogue = kElementHandleInvalid;
	m_currentMouseover = kElementHandleInvalid;
	m_currentFocus = kElementHandleInvalid;
}

Dusk::UserInterface::~UserInterface ( void )
{
	// Remove all the elements
	for (size_t i = 0; i < m_elements.size(); ++i)
	{
		delete m_elements[i];
		m_elements[i] = NULL;
	}
	m_elements.clear();

	// Remove the renderer last
	delete m_renderer;
	m_renderer = NULL;
}

//	Update() : Called by engine.
// Performs per-frame behavior updates of the UI, generates layouts, and sets up information
// needed for display.
void Dusk::UserInterface::Update ( void )
{
	// Delete objects that need to be deleted
	{
		std::vector<size_t> delete_list;
		for ( size_t i = 0; i < m_elements.size(); ++i )
		{
			if ( m_elements[i] != NULL && m_elements[i]->m_destructionRequested )
			{
				delete_list.push_back(i);
			}
		}
		for ( unsigned int i = 0; i < delete_list.size(); ++i )
		{
			DestroyElement(delete_list[i]);
		}
	}

	UpdateMouseOver();
	UpdateFocus();
	UpdateElements();
}


void Dusk::UserInterface::UpdateMouseOver ( void )
{
	// Reset mouseover
	m_currentMouseover = kElementHandleInvalid;

	std::vector<uint32_t> mouseoverList (m_elements.size());
	std::vector<uint32_t> mouseoverListFinal (m_elements.size());

	for ( unsigned int i = 0; i < m_elements.size(); ++i )
	{
		if ( m_elements[i] == NULL ) // Skip deleted
			continue;
		if ( !m_elements[i]->m_visible ) // Skip invisible
			continue;
		if ( !m_elements[i]->m_wasDrawn ) // Skip not drawn
			continue;

		// Add current object into list if mouse is in it
		if ( m_elements[i]->m_isMouseIn )
		{
			mouseoverList.push_back(i);
			mouseoverListFinal.push_back(i);
		}
	}

	// Add in the entire parent tree
	for ( unsigned int i = 0; i < mouseoverList.size(); ++i )
	{
		Dusk::Element* currentParent = m_elements[mouseoverList[i]]->m_parent;
		if (currentParent != NULL)
		{
			// First check that it's not in already
			if ( std::find( mouseoverList.begin(), mouseoverList.end(), currentParent ) == mouseoverList.end() )
			{
				ARCORE_ASSERT(currentParent == m_elements[currentParent->m_index]);
				mouseoverList.push_back( currentParent->m_index );
				mouseoverListFinal.push_back( currentParent->m_index );
			}
		}
	}

	// Loop through list and remove any parents (INCORRECT)
	for ( unsigned int i = 0; i < mouseoverList.size(); ++i )
	{
		for ( unsigned int j = 0; j < mouseoverListFinal.size(); ++j )
		{
			Dusk::Element* currentParent = m_elements[mouseoverList[i]]->m_parent;
			if ( currentParent != NULL && currentParent->m_index == mouseoverListFinal[j] )
			{
				mouseoverListFinal.erase( mouseoverListFinal.begin()+j );
				j -= 1;
			}
		}
	}

	// Update the mouseover if with the first found item.
	if ( mouseoverList.size() > 0 )
	{
		m_currentMouseover = mouseoverListFinal[0];
	}
}

void Dusk::UserInterface::UpdateFocus ( void )
{
	// On mouse click, focus the item the mouse is over.
	if ( Input::MouseDown(Input::MBLeft) )
	{
		// Unfocus the current focus
		if (m_currentFocus != kElementHandleInvalid && m_elements[m_currentFocus] != NULL)
		{
			m_elements[m_currentFocus]->m_isFocused = false;
		}

		// Redundancy: Reset the focus of all other elements
		for ( unsigned int i = 0; i < m_elements.size(); ++i )
		{
			if ( m_elements[i] != NULL ) {
				m_elements[i]->m_isFocused = false;
			}
		}

		// Set the current focus
		m_currentFocus = m_currentMouseover;
		if (m_currentFocus != kElementHandleInvalid && m_elements[m_currentFocus] != NULL)
		{
			m_elements[m_currentFocus]->m_isFocused = true;
		}
	}

	// If not in dialogue mode...
	if ( m_currentDialogue == kElementHandleInvalid )
	{
		// Now, if there's focus, and tab is hit, cycle through the elements
		if ( Input::Keydown( Keys.Tab ) )
		{
			int repeatCount = 0; // Variable for preventing infinite loops.

			uint32_t nextFocus = m_currentFocus;
			if ( m_currentFocus == kElementHandleInvalid ) {
				nextFocus = kElementHandleInvalid;
			}

			// Cycle to next valid element
			bool keepCycling = true;
			while ( keepCycling )
			{
				nextFocus += 1;
				if ( nextFocus >= m_elements.size() )
				{
					nextFocus = 0;
					// Limit infinite loops, in the case there is nothing to focus to.
					if ( repeatCount++ > 2 )
					{
						nextFocus = m_currentFocus;
						break;
					}
				}
				if ( m_elements[nextFocus] != NULL )
				{
					if ( m_elements[nextFocus]->m_visible && m_elements[nextFocus]->m_wasDrawn && m_elements[nextFocus]->m_canFocus )
					{
						keepCycling = false;
					}
					if ( m_currentFocus == nextFocus )
					{
						keepCycling = false;
					}
				}
			}

			// Unfocus the current focus
			if (m_currentFocus != kElementHandleInvalid && m_elements[m_currentFocus] != NULL)
			{
				m_elements[m_currentFocus]->m_isFocused = false;
			}

			// Redundancy: Reset the focus of all other elements
			for ( unsigned int i = 0; i < m_elements.size(); ++i )
			{
				if ( m_elements[i] != NULL ) {
					m_elements[i]->m_isFocused = false;
				}
			}

			// Set the current focus
			m_currentFocus = nextFocus;
			if (m_currentFocus != kElementHandleInvalid && m_elements[m_currentFocus] != NULL)
			{
				m_elements[m_currentFocus]->m_isFocused = true;
			}
		}
		// End tab cycling
	}
	// If in dialogue mode...
	else
	{
		// Fix the focus
		m_currentFocus = m_currentDialogue;

		// Reset focuses
		for ( unsigned int i = 0; i < m_elements.size(); ++i )
		{
			if ( m_elements[i] != NULL )
			{
				m_elements[i]->m_isFocused = false;
				m_elements[i]->m_isMouseIn = false;
				m_elements[i]->m_isEnabled = false;
			}
		}

		if (m_currentFocus != kElementHandleInvalid && m_elements[m_currentFocus] != NULL)
		{
			m_elements[m_currentFocus]->m_isFocused = true;
			m_elements[m_currentFocus]->m_isEnabled = true;
		}
	}
}

void Dusk::UserInterface::UpdateElements ( void )
{
	if ( m_currentDialogue == kElementHandleInvalid )
	{
		// Reset offsets
		std::vector<bool> offsetListGenerated(m_elements.size(), false);
		std::vector<Vector2f>& offsetList = m_updateOffsets;
		offsetList.resize(m_elements.size(), Vector2f(0,0));

		// Iterate through all the components
		for ( unsigned int i = 0; i < m_elements.size(); ++i )
		{
			Element* currentElement = m_elements[i];
			m_currentElement = i;

			if (currentElement == NULL)
				continue; // Skip invalid elements

			if (!offsetListGenerated[i])
			{
				// If we have a parent, we need to generate the offset of this element
				if ( currentElement->m_parent != NULL )
				{
					// Create the offset generation stack, which starts with the current element.
					std::vector<Element*> offsetGenerationStack;
					offsetGenerationStack.push_back(currentElement);

					// Push back the entire parent tree to the request of what to generate.
					Element* currentParent = currentElement->m_parent;
					while (true)
					{
						ARCORE_ASSERT(m_elements[currentParent->m_index] == currentParent);
						if (currentParent == NULL || offsetListGenerated[currentParent->m_index])
								break;
						offsetGenerationStack.push_back(currentParent);
						currentParent = currentParent->m_parent;
					}
					
					// Generate the offsets
					for (int i = (int)(offsetGenerationStack.size() - 1); i > 1; ++i)
					{
						//currentParent = offsetGenerationStack[i];
						Element* currentChild = offsetGenerationStack[i];
						currentParent = currentChild->m_parent;
						
						if (currentParent == NULL)
						{	// If no parent, then there's no offset
							offsetList[currentChild->m_index] = Vector2f(0,0);
							offsetListGenerated[currentChild->m_index] = true;
						}
						else
						{	// If there is a parent, then the offset is their local position combined w/ their offset.
							offsetList[currentChild->m_index] = currentParent->m_parent->m_localRect.pos + offsetList[currentParent->m_parent->m_index];
							offsetListGenerated[currentChild->m_index] = true;
						}
					}
					// The current element's offset will be generated by the previous loop.
				}
				else
				{	// If no parent, then there's no offset
					offsetList[i] = Vector2f(0,0);
					offsetListGenerated[i] = true;
				}
			}

			// Update the offset'd rect of the element
			currentElement->m_absoluteRect = Rect(offsetList[i] + currentElement->m_localRect.pos, currentElement->m_localRect.size);

			// Update them
			currentElement->Update();
		}
	}
	else
	{
		// Set current element
		m_currentElement = m_currentDialogue;

		Element* currentElement = m_elements[m_currentElement];

		// Update only the offset'd rect of the dialogue box
		currentElement->m_absoluteRect = Rect(m_updateOffsets[m_currentElement] + currentElement->m_localRect.pos, currentElement->m_localRect.size);

		// Update them
		currentElement->Update();
	}
}

//	DestroyElement() : Destroys the element with the given index/handle.
// Used internally to remove elements.
void Dusk::UserInterface::DestroyElement ( const size_t handle )
{
	if ( handle >= m_elements.size() )
	{
		throw core::InvalidArgumentException();
	}
	if ( m_elements[handle] != NULL )
	{
		// Force an update of the UI that this element affected.
		m_forcedUpdateAreas.push_back( m_elements[handle]->m_absoluteRect );
		// If the element was a dialogue, though, it likely affected the entire screen.
		if ((size_t)m_currentDialogue == handle)
		{
			m_forcedUpdateAreas.push_back( Rect(0,0, (Real)Screen::Info.width, (Real)Screen::Info.height) );
		}

		// Remove the elements
		delete m_elements[handle];
		m_elements[handle] = NULL;
	}
}

//	EnterDialogue(element) : Enters the element as a dialogue.
void Dusk::UserInterface::EnterDialogue ( Element* element )
{
	ARCORE_ASSERT(m_elements[element->m_index] == element);

	m_currentDialogue = element->m_index;

	// Immediately reset all focuses
	for ( unsigned int i = 0; i < m_elements.size(); ++i )
	{
		if ( m_elements[i] != NULL )
		{
			m_elements[i]->m_isFocused = false;
			m_elements[i]->m_isMouseIn = false;
			m_elements[i]->m_isEnabled = false;
		}
	}
}

//	ExitDialogue(element) : Exit dialogue mode of the system.
void Dusk::UserInterface::ExitDialogue ( Element* element )
{
	ARCORE_ASSERT(m_elements[element->m_index] == element);
	ARCORE_ASSERT(m_currentDialogue == element->m_index);

	m_currentDialogue = kElementHandleInvalid;

	// Immediately re-enable all focuses
	for ( unsigned int i = 0; i < m_elements.size(); ++i )
	{
		if ( m_elements[i] != NULL ) {
			m_elements[i]->m_isEnabled = true;
		}
	}
}