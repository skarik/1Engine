#include "UI.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "renderer/windowing/RrWindow.h"

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

dusk::UserInterface::UserInterface ( RrWindow* targeted_display, RrWorld* targeted_world )
	: CGameBehavior()
	, m_window(targeted_display)
{
	m_renderer = new UIRenderer(this);
	if (targeted_world != nullptr)
	{
		m_renderer->AddToWorld(targeted_world);
	}

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

	// Clear up the tree
	ClearElementTree();
	delete m_elementTreeBase;
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

	m_treeInUse = true;

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
			if ( element->GetElementType() == ElementType::kControl )
			{
				if ( !element->m_visible ) // Skip invisible
					continue;
				if ( !element->m_wasDrawn ) // Skip not drawn
					continue;

				if (element->m_isMouseIn)
				{
					m_currentMouseover = element->m_index;
				}
			}
		}

		// Update children of this item last so we update the mouse over in tiers
		for (ElementNode* child : elementNode->children)
		{
			updateList.push_back(child);
		}
	}

	m_treeInUse = false;
}

void dusk::UserInterface::UpdateFocus ( void )
{
	const int input_index = m_window ? m_window->GetListIndex() : -1;

	// On mouse click, focus the item the mouse is over.
	if ( core::Input::MouseDown(core::kMBLeft, input_index) )
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
		if ( core::Input::Keydown( core::kVkTab, input_index ) )
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
	/*else
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
	}*/
}

void dusk::UserInterface::ClearElementTreeFromNode(ElementNode* rootNode)
{
	std::vector<ElementNode*> nodesToTraverse;
	nodesToTraverse.reserve(m_elements.size());

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

void dusk::UserInterface::ClearElementTree ( void )
{
	if (m_treeInUse)
	{
		// Queue all items in the tree base.
		for (auto child : m_elementTreeBase->children)
		{
			m_treeNodeDestroyQueue.push_back(child);
		}
		m_elementTreeBase->children.clear();
	}
	else
	{
		// Clear everything in the tree base.
		for (auto child : m_elementTreeBase->children)
		{
			ClearElementTreeFromNode(child);
		}
		m_elementTreeBase->children.clear();

		// Clear everything in the destroy queue
		for (auto child : m_treeNodeDestroyQueue)
		{
			ClearElementTreeFromNode(child);
		}
		m_treeNodeDestroyQueue.clear();
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
	//if ( m_currentDialogue == kElementHandleInvalid )
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
	/*else
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
	}*/
}

void dusk::UserInterface::UpdateElements ( void )
{
	const int input_index = m_window ? m_window->GetListIndex() : -1;

	dusk::UIStepInfo l_stepInfo;
	l_stepInfo.mouse_position = Vector2f((Real)core::Input::MouseX(input_index), (Real)core::Input::MouseY(input_index));
	l_stepInfo.input_index = input_index;

	m_treeInUse = true;
	//if ( m_currentDialogue == kElementHandleInvalid )
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
				ARCORE_ASSERT(elementNode->index < m_elements.size());
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
	} // TODO: Mask out only the items on the dialog button tree
	/*else
	{
		// Set current element
		m_currentElement = m_currentDialogue;

		Element* element = m_elements[m_currentElement];

		// Update them
		element->Update(&l_stepInfo);
	}*/
	m_treeInUse = false;

	// Update the renderer's mouse position
	m_renderer->m_glowPosition = l_stepInfo.mouse_position;
}

//	DestroyElement(index) : Removes the element with the given index/handle.
// Should only be used in the Element destructor.
void dusk::UserInterface::RemoveElement ( const size_t handle )
{
	DestroyElement(handle, false);
}

//	DestroyElement(index) : Destroys the element with the given index/handle.
// Used internally to remove elements.
void dusk::UserInterface::DestroyElement ( const size_t handle, const bool also_delete )
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
			m_forcedUpdateAreas.push_back( Rect(0,0, (Real)GetScreen().GetWidth(), (Real)GetScreen().GetHeight()) );
		}

		// Remove the elements
		if (also_delete)
		{
			delete m_elements[handle];
		}
		m_elements[handle] = NULL;

		// Request a regen of the element tree
		m_treeNeedsGeneration = true;
	}
}

void dusk::UserInterface::PushEnableState ( void )
{
	m_dialogueActivationStack.push_back(std::vector<SavedEnableState>());
	// Save current state of focuses
	m_dialogueActivationStack.back().resize(m_elements.size());
	for ( unsigned int i = 0; i < m_elements.size(); ++i )
	{
		if ( m_elements[i] != NULL )
		{
			m_dialogueActivationStack.back()[i] = {m_elements[i], i, m_elements[i]->m_isEnabled};
		}
	}
}

void dusk::UserInterface::PopEnableState ( void )
{
	ARCORE_ASSERT(!m_dialogueActivationStack.empty());

	auto& activationState = m_dialogueActivationStack.back();
	for (SavedEnableState& stateInfo : activationState)
	{
		// If it's a proper match, then simply restore the state
		if ( stateInfo.index < m_elements.size() && m_elements[stateInfo.index] == stateInfo.element )
		{
			stateInfo.element->m_isEnabled = stateInfo.enabled;
		}
		else 
		{
			// Find the element in the listing
			for (Element* element : m_elements)
			{
				if ( stateInfo.element == element )
				{
					stateInfo.element->m_isEnabled = stateInfo.enabled;
					break;
				}
			}
		}
		// Anything else is an invalid object and should be skipped, as it may have been since deleted.
	}
	m_dialogueActivationStack.pop_back();
}

//	EnterDialogue(element) : Enters the element as a dialogue.
void dusk::UserInterface::EnterDialogue ( Element* element )
{
	ARCORE_ASSERT(m_elements[element->m_index] == element);

	PushEnableState();

	// Set current dialog state
	m_currentDialogue = element->m_index;

	// Immediately disable all focuses on elements that are not part of the dialogue
	GenerateElementTree(); // Tree needs to be updated for navigating thru the elements
	{
		std::list<ElementNode*> updateList;
		updateList.push_front(m_elementTreeBase);

		while (!updateList.empty())
		{
			ElementNode* elementNode = updateList.front();
			updateList.pop_front();

			if (elementNode->index == m_currentDialogue)
			{
				continue; // Skip the current dialog and its children entirely.
			}

			if (elementNode->index != kElementHandleInvalid)
			{
				Element* element = m_elements[elementNode->index];
				ARCORE_ASSERT(element->m_index == elementNode->index);

				element->m_isFocused = false;
				element->m_isMouseIn = false;
				element->m_isEnabled = false;
			}

			// Disable children of this item as well
			for (ElementNode* child : elementNode->children)
			{
				updateList.push_front(child);
			}
		}
	}
}

//	ExitDialogue(element) : Exit dialogue mode of the system.
void dusk::UserInterface::ExitDialogue ( Element* element )
{
	ARCORE_ASSERT(m_elements[element->m_index] == element);
	ARCORE_ASSERT(m_currentDialogue == element->m_index);
	ARCORE_ASSERT(!m_dialogueActivationStack.empty());

	// Clear state that gets skipped
	element->m_isMouseIn = false;
	element->m_isFocused = false;

	m_currentDialogue = kElementHandleInvalid;

	PopEnableState();
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

//	IsInDialogue() : Checks if dialogue is currently open
bool dusk::UserInterface::IsInDialogue ( void )
{
	return m_currentDialogue != kElementHandleInvalid;
}

//	GetScreen() : Returns the screen associated with this UI.
const ArScreen& dusk::UserInterface::GetScreen ( void )
{
	return m_window->GetScreen();
}