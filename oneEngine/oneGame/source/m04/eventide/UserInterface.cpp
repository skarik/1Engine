#include "UserInterface.h"

#include "core/debug/Console.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "../eventide/Element.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/texture/RrFontTexture.h"

#include "renderer/camera/RrCamera.h"

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

		// Avoid referencing the removed element in states:

		if (m_currentFocusedElement == element)
		{
			m_currentFocusedElement = nullptr;
		}
		if (m_currentMouseOverElement == element)
		{
			m_currentMouseOverElement = nullptr;
		}

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
	for (ui::eventide::Element* element : m_elements)
	{
		if (element->m_parent == NULL)
		{
			// Take the base bbox.
			element->m_bboxAbsolute = element->m_bbox;
		}
		else
		{
			// Start with base bbox
			element->m_bboxAbsolute = element->m_bbox;
			// Transform by the parent's matrix
			element->m_bboxAbsolute.m_M			= element->m_bboxAbsolute.m_M * element->m_parent->m_bboxAbsolute.m_M;
			element->m_bboxAbsolute.m_MInverse	= element->m_parent->m_bboxAbsolute.m_MInverse	* element->m_bboxAbsolute.m_MInverse;
		}
	}

	// Update tab-focus:
	if (Input::Key(VK_TAB))
	{
		Element* nextFocusedElement = nullptr;

		// Find the position of the element in the list:
		auto elementIterator = std::find(m_elements.begin(), m_elements.end(), m_currentFocusedElement);
		if (elementIterator == m_elements.end())
		{
			elementIterator = m_elements.begin();
		}

		// Increment the iterator until we find an entry that can be highlighted
		for (++elementIterator; elementIterator != m_elements.end(); ++elementIterator)
		{
			ARCORE_ASSERT(*elementIterator != NULL);
			if ((*elementIterator)->GetFocusInteract() == ui::eventide::Element::FocusInteract::kFocusable)
			{
				nextFocusedElement = *elementIterator;
				break;
			}
		}

		// Update cycle-focus state now
		if (m_currentFocusedElement == nextFocusedElement || nextFocusedElement == NULL)
		{
			if (m_currentFocusedElement != NULL)
			{
				m_currentFocusedElement->m_focused = false;
			}
			m_currentFocusedElement = NULL;
		}
		else
		{
			if (m_currentFocusedElement != NULL)
			{
				m_currentFocusedElement->m_focused = false;
			}
			m_currentFocusedElement = nextFocusedElement;
			m_currentFocusedElement->m_focused = true;
		}
	}

	// Update mouse clickity clack:
	if (RrCamera::activeCamera != NULL)
	{
		const Vector2f mouseScreenPosition (Input::MouseX() / Screen::Info.width, Input::MouseY() / Screen::Info.height);
		const Ray mouseRay = Ray(
			RrCamera::activeCamera->transform.position,
			RrCamera::activeCamera->ScreenToWorldDir(mouseScreenPosition)
			);
		
		// find element with the shortest distance to the camera. that's the one with the mouse focus

		float minMouseHitDistance = INFINITY;
		Element* minMouseHitElement = NULL;

		for (ui::eventide::Element* element : m_elements)
		{
			ARCORE_ASSERT(element != NULL);

			const ui::eventide::Element::MouseInteract elementInteractStyle = element->GetMouseInteract();

			if (elementInteractStyle == ui::eventide::Element::MouseInteract::kBlocking
				|| elementInteractStyle == ui::eventide::Element::MouseInteract::kCapturing)
			{
				float mouseHitDistance = 0.0F;
				if (element->GetBBoxAbsolute().Raycast(mouseRay, mouseHitDistance))
				{
					if (minMouseHitDistance > mouseHitDistance)
					{
						minMouseHitDistance = mouseHitDistance;
						minMouseHitElement = element;
					}
				}
			}
		}

		// Update changes in the state and generate events:

		// Common event to all elements
		ui::eventide::Element::EventMouse event;
		event.position_screen = mouseScreenPosition;
		event.position_world = mouseRay.pos + mouseRay.dir * minMouseHitDistance;

		if (minMouseHitElement != m_currentMouseOverElement)
		{
			// Exiting the current element:
			if (m_currentMouseOverElement != NULL)
			{
				if (m_currentMouseOverElement->GetMouseInteract() == ui::eventide::Element::MouseInteract::kCapturing)
				{
					event.type = ui::eventide::Element::EventMouse::Type::kExit;
					m_currentMouseOverElement->OnEventMouse(event);
				}
				m_currentMouseOverElement->m_mouseInside = false;
			}

			// Set new mouse-over:
			m_currentMouseOverElement = minMouseHitElement;

			// Entering the new element:
			if (m_currentMouseOverElement != NULL)
			{
				if (m_currentMouseOverElement->GetMouseInteract() == ui::eventide::Element::MouseInteract::kCapturing)
				{
					event.type = ui::eventide::Element::EventMouse::Type::kEnter;
					m_currentMouseOverElement->OnEventMouse(event);
				}
				m_currentMouseOverElement->m_mouseInside = true;
			}
		}
		else
		{
			// Staying inside the current element:
			if (Input::MouseDown(Input::MBLeft))
			{
				event.type = ui::eventide::Element::EventMouse::Type::kClicked;
				event.button = Input::MBLeft;
				m_currentMouseOverElement->OnEventMouse(event);
			}
			if (Input::MouseDown(Input::MBRight))
			{
				event.type = ui::eventide::Element::EventMouse::Type::kClicked;
				event.button = Input::MBRight;
				m_currentMouseOverElement->OnEventMouse(event);
			}
			if (Input::MouseDown(Input::MBMiddle))
			{
				event.type = ui::eventide::Element::EventMouse::Type::kClicked;
				event.button = Input::MBMiddle;
				m_currentMouseOverElement->OnEventMouse(event);
			}
		}
	}

	// Do other input events:
	for (ui::eventide::Element* element : m_elements) // TODO: this can be parallelized, probably??? Depends on the element OnThing calls.
	{
		const uint32_t interactMask = element->GetInputInteractMask();
		if (element->m_focused || (interactMask & ui::eventide::Element::InputInteractMasks::kCatchAll))
		{
			if ((interactMask & ui::eventide::Element::InputInteractMasks::kActivateEnter)
				&& Input::Keydown(VK_RETURN))
			{
				ui::eventide::Element::EventInput event;
				event.type = ui::eventide::Element::EventInput::Type::kActivate;
				element->OnEventInput(event);
			}

			if ((interactMask & ui::eventide::Element::InputInteractMasks::kActivateSpace)
				&& Input::Keydown(VK_SPACE))
			{
				ui::eventide::Element::EventInput event;
				event.type = ui::eventide::Element::EventInput::Type::kActivate;
				element->OnEventInput(event);
			}
		}
	}
}

ui::eventide::Texture ui::eventide::UserInterface::LoadTexture ( const char* filename )
{
	// Load texture
	RrTexture* loaded_texture = RrTexture::Load(filename);
	
	// Check the internal texture list and add if needed
	auto texture_location = std::find(m_textures.begin(), m_textures.end(), loaded_texture);
	if (texture_location == m_textures.end())
	{
		loaded_texture->AddReference();
		m_textures.push_back(loaded_texture);
	}

	return ui::eventide::Texture{loaded_texture, (uint32_t)std::distance(m_textures.begin(), texture_location)};
}

ui::eventide::Texture ui::eventide::UserInterface::LoadTextureFont ( const char* filename )
{
	// Set load params
	rrFontTextureLoadParams params;
	params.characterSets = 0;
	params.height = 24;
	params.weight = kFW_Normal;

	// Load texture
	RrFontTexture* loaded_texture = RrFontTexture::Load(filename, params);

	// Check the internal texture list and add if needed
	auto texture_location = std::find(m_textures.begin(), m_textures.end(), loaded_texture);
	if (texture_location == m_textures.end())
	{
		loaded_texture->AddReference();
		m_textures.push_back(loaded_texture);
	}

	return ui::eventide::Texture{loaded_texture, (uint32_t)std::distance(m_textures.begin(), texture_location)};
}

void ui::eventide::UserInterface::ReleaseTexture ( const Texture& texture )
{
	auto find_result = std::find(m_textures.begin(), m_textures.end(), texture.reference);
	if (find_result != m_textures.end())
	{
		texture.reference->RemoveReference();
		m_textures.erase(find_result);
	}
}
