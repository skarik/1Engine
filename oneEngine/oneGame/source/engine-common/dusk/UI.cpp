#include "UI.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "engine-common/dusk/Element.h"
#include "engine-common/dusk/UIRenderer.h"

//	AddInitialize(element, desc) : Initializes the element with the given description.
// Used after an element is instantiated, applies the given parameters.
dusk::Element* dusk::UserInterface::AddInitialize ( Element* element, const ElementCreationDescription& desc )
{
	element->m_interface = this; // Take ownership of this element

	element->m_localRect	= desc.localRect;

	if (desc.parent != NULL)
	{
		// Find the input parent within the given list
		uint32_t parentIndex = kElementHandleInvalid;
		for (uint32_t i = 0; i < m_elements.size(); ++i)
		{
			if (m_elements[i] == desc.parent)
			{
				parentIndex = i;
				break;
			}
		}
		if (parentIndex != kElementHandleInvalid)
		{
			element->m_parent = desc.parent;
		}
		else
		{
			throw core::InvalidReferenceException();
		}
	}
	else
	{
		element->m_parent = NULL;
	}

	// Update index
	element->m_index = (uint32_t)m_elements.size();

	return element;
}

//	AddInitializeCheckValid(element) : Checks the given element has the correct index.
// To be used as part of an assertion check.
bool dusk::UserInterface::AddInitializeCheckValid ( Element* element )
{
	return m_elements[element->m_index] == element;
}

dusk::UserInterface::UserInterface ( void )
	: CGameBehavior()
{
	m_renderer = new UIRenderer(this);

	// Handle init
	m_currentElement = kElementHandleInvalid;
	m_currentDialogue = kElementHandleInvalid;
	m_currentMouseover = kElementHandleInvalid;
	m_currentFocus = kElementHandleInvalid;

	// Set up tree root
	m_elementTreeBase = new ElementNode;
	m_elementTreeBase->index = kElementHandleInvalid;
}

dusk::UserInterface::~UserInterface ( void )
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
void dusk::UserInterface::Update ( void )
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

	GenerateElementTree();
	UpdateElementPositions();
	UpdateMouseOver();
	UpdateFocus();
	UpdateElements();
}


void dusk::UserInterface::UpdateMouseOver ( void )
{
	// Reset mouseover
	m_currentMouseover = kElementHandleInvalid;

	/*std::vector<uint32_t> mouseoverList (m_elements.size());
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
		dusk::Element* currentParent = m_elements[mouseoverList[i]]->m_parent;
		if (currentParent != NULL)
		{
			// First check that it's not in already
			if ( std::find( mouseoverList.begin(), mouseoverList.end(), currentParent->m_index ) == mouseoverList.end() )
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
			dusk::Element* currentParent = m_elements[mouseoverList[i]]->m_parent;
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
	}*/

	// Update positions, going down the tree.
	std::list<ElementNode*> updateList;
	updateList.push_front(m_elementTreeBase);

	while (!updateList.empty())
	{
		ElementNode* elementNode = updateList.front();
		updateList.pop_front();

		if (elementNode->index != kElementHandleInvalid)
		{
			Element* element = m_elements[elementNode->index];
			ARCORE_ASSERT(element->m_index == elementNode->index);

			if ( element == NULL ) // Skip deleted
				continue;
			if ( !element->m_visible ) // Skip invisible
				continue;
			if ( !element->m_wasDrawn ) // Skip not drawn
				continue;

			if (element->m_isMouseIn)
			{
				m_currentMouseover = element->m_index;
			}
		}

		// Update children of this item last so we update the mouse over in tiers
		for (ElementNode* child : elementNode->children)
		{
			updateList.push_back(child);
		}
	}
}

void dusk::UserInterface::UpdateFocus ( void )
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
		
		// Mouse is used, so don't need to draw the focus
		m_renderer->m_renderMouselessFocus = false;
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

			// Tab cycling happened, so enable the visual focus
			m_renderer->m_renderMouselessFocus = true;
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

void dusk::UserInterface::ClearElementTree ( void )
{
	std::vector<ElementNode*> nodesToTraverse;
	nodesToTraverse.reserve(m_elements.size());

	for (auto child : m_elementTreeBase->children)
	{
		nodesToTraverse.push_back(child);
	}
	m_elementTreeBase->children.clear();

	for (int traverseIndex = 0; traverseIndex < nodesToTraverse.size(); ++traverseIndex)
	{
		ElementNode* currentNode = nodesToTraverse[traverseIndex];
		ARCORE_ASSERT(currentNode != NULL);

		// Push back all children
		for (auto child : currentNode->children)
		{
			nodesToTraverse.push_back(child);
		}

		// Delete now that we done need it.
		delete currentNode;
	}
}

void dusk::UserInterface::GenerateElementTree ( void )
{
	if (m_treeNeedsGeneration)
	{
		// Clear out current tree
		//m_elementTreeBase.children.clear();
		ClearElementTree();

		std::vector<bool> addedToTree(m_elements.size(), false);
		std::vector<ElementNode*> treeNodeLookup(m_elements.size(), NULL);

		// Iterate through all the components
		for (unsigned int i = 0; i < m_elements.size(); ++i)
		{
			Element* currentElement = m_elements[i];
			m_currentElement = i;

			if (currentElement == NULL)
				continue; // Skip invalid elements

			if (!addedToTree[i])
			{
				// If no parent, then just add to the tree's base
				if (currentElement->m_parent == NULL)
				{	
					m_elementTreeBase->children.push_back(new ElementNode());
					m_elementTreeBase->children.back()->index = i;

					treeNodeLookup[i] = m_elementTreeBase->children.back();
					addedToTree[i] = true;
				}
				// If we have a parent, we need to generate the the dependancy tree of this element
				else
				{	
					// Create the parenting stack, which starts with the current element.
					std::vector<Element*> treeGenerationStack;
					treeGenerationStack.push_back(currentElement);

					// Push back the entire parent tree to the request of what to generate.
					Element* currentParent = currentElement->m_parent;
					while (currentParent != NULL && !addedToTree[currentParent->m_index])
					{
						ARCORE_ASSERT(m_elements[currentParent->m_index] == currentParent);
						treeGenerationStack.push_back(currentParent);
						currentParent = currentParent->m_parent;
					}

					// Generate the offsets, starting at the parent and working down the list.
					for (auto stackItr = treeGenerationStack.rbegin(); stackItr != treeGenerationStack.rend(); ++stackItr)
					{
						Element* currentChild = *stackItr;
						currentParent = currentChild->m_parent;

						ARCORE_ASSERT(currentChild != NULL && m_elements[currentChild->m_index] == currentChild);

						// If no parent, then add them to the trunk
						if (currentParent == NULL)
						{	
							m_elementTreeBase->children.push_back(new ElementNode());
							m_elementTreeBase->children.back()->index = currentChild->m_index;

							treeNodeLookup[currentChild->m_index] = m_elementTreeBase->children.back();
							addedToTree[currentChild->m_index] = true;
						}
						// If there is a parent, then add the current child to its node
						else
						{	
							ARCORE_ASSERT(addedToTree[currentParent->m_index]);
							ElementNode* parentNode = treeNodeLookup[currentParent->m_index];

							parentNode->children.push_back(new ElementNode());
							parentNode->children.back()->index = currentChild->m_index;

							treeNodeLookup[currentChild->m_index] = parentNode->children.back();
							addedToTree[currentChild->m_index] = true;
						}
					}
					// The current element's offset will be generated by the previous loop.
				}
			}
		}

		// No longer needs generation
		m_treeNeedsGeneration = false;
	}
}

void dusk::UserInterface::UpdateElementPositions ( void )
{
	if ( m_currentDialogue == kElementHandleInvalid )
	{
		// Update positions, going down the tree.
		std::list<ElementNode*> updateList;
		updateList.push_front(m_elementTreeBase);

		while (!updateList.empty())
		{
			ElementNode* elementNode = updateList.front();
			updateList.pop_front();

			// Grab the current element
			Element* element = NULL;
			if (elementNode->index != kElementHandleInvalid)
			{
				element = m_elements[elementNode->index];
				ARCORE_ASSERT(element->m_index == elementNode->index);
			}

			// Do the layout-type elements
			if (element != NULL && element->m_elementType == ElementType::kLayout)
			{
				std::vector<Element*> children;
				for (ElementNode* childNode : elementNode->children)
				{
					Element* child = m_elements[childNode->index];
					if (!child->m_overrideLayout && !child->m_ignoreAutoLayout)
					{	// Add the child element to the update list for the layout
						children.push_back(child);
					}

					// Update children of this item before others now
					updateList.push_front(childNode);
				}
				element->as<dusk::LayoutElement>()->LayoutChildren(children); // Actually execute children layouts now
			}
			// Do the control-type elements
			else
			{
				for (ElementNode* childNode : elementNode->children)
				{
					Element* child = m_elements[childNode->index];
					
					if (!child->m_overrideLayout)
					{
						if (element == NULL)
						{
							child->m_absoluteRect = child->m_localRect;
						}
						else
						{	// Update the offset'd rect of the element
							child->m_absoluteRect = Rect(element->m_absoluteRect.pos + child->m_localRect.pos, child->m_localRect.size);
						}
					}

					// Update this child item before others now
					updateList.push_front(childNode);
				}
			}
		}
	}
	else
	{
		// Set current element
		m_currentElement = m_currentDialogue;

		Element* element = m_elements[m_currentElement];
		Element* parent = element->m_parent;

		// Update only the offset'd rect of the dialogue box
		if (parent == NULL)
		{
			element->m_absoluteRect = element->m_localRect;
		}
		else
		{
			element->m_absoluteRect = Rect(parent->m_absoluteRect.pos + element->m_localRect.pos, element->m_localRect.size);
		}
	}
}

void dusk::UserInterface::UpdateElements ( void )
{
	dusk::UIStepInfo l_stepInfo;
	l_stepInfo.mouse_position = Vector2f((Real)Input::MouseX(), (Real)Input::MouseY());

	if ( m_currentDialogue == kElementHandleInvalid )
	{
		// Update positions, going down the tree.
		std::list<ElementNode*> updateList;
		updateList.push_front(m_elementTreeBase);

		while (!updateList.empty())
		{
			ElementNode* elementNode = updateList.front();
			updateList.pop_front();

			if (elementNode->index != kElementHandleInvalid)
			{
				Element* element = m_elements[elementNode->index];
				ARCORE_ASSERT(element->m_index == elementNode->index);
				
				// Update them
				element->Update(&l_stepInfo);
			}

			// Update children of this item before others now
			for (ElementNode* child : elementNode->children)
			{
				updateList.push_front(child);
			}
		}
	}
	else
	{
		// Set current element
		m_currentElement = m_currentDialogue;

		Element* element = m_elements[m_currentElement];

		// Update them
		element->Update(&l_stepInfo);
	}
}

//	DestroyElement() : Destroys the element with the given index/handle.
// Used internally to remove elements.
void dusk::UserInterface::DestroyElement ( const size_t handle )
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

		// Request a regen of the element tree
		m_treeNeedsGeneration = true;
	}
}

//	EnterDialogue(element) : Enters the element as a dialogue.
void dusk::UserInterface::EnterDialogue ( Element* element )
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
void dusk::UserInterface::ExitDialogue ( Element* element )
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

//	IsMouseInside() : Checks if mouse cursor is currently inside any active element
bool dusk::UserInterface::IsMouseInside ( void )
{
	return std::any_of(
		m_elements.begin(), m_elements.end(),
		[](dusk::Element* element)
		{ 
			return element != NULL && element->m_isMouseIn;
		}
	);
}