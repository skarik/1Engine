#include "UserInterface.h"

#include "core/debug/Console.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "core-ext/threads/Jobs.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/camera/RrCamera.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/material/RrShaderProgram.h"

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
	// Create renderable
	m_renderable = new CStreamedRenderable3D();

	RrPass uiPass;
	uiPass.utilSetupAsDefault();
	uiPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/eventide_default_vv.spv", "shaders/v2d/eventide_default_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor,
											renderer::shader::Location::kNormal,
											renderer::shader::Location::kUV1};
	m_renderable->PassInitWithInput(0, &uiPass);
}

ui::eventide::UserInterface::~UserInterface ( void )
{
	delete m_renderable;

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


struct MeshOffsets
{
	uint32_t	indexOffset;
	uint32_t	vertexOffset;
};

//	PostStep() : Threaded post-render
void ui::eventide::UserInterface::PostStep ( void )
{
	std::vector<MeshOffsets> l_meshOffsets (m_elements.size());
	MeshOffsets l_trackedOffset = {0, 0};

	// Generate the offsets for each mesh
	for (uint32_t elementIndex = 0; elementIndex < m_elements.size(); ++elementIndex)
	{
		const Element* element = m_elements[elementIndex];
		ARCORE_ASSERT(element != NULL);

		// Skip in-progress meshes
		if (element->mesh_creation_state.building_mesh)
		{
			continue;
		}

		// Save current offset
		l_meshOffsets[elementIndex] = l_trackedOffset;

		// Generate next offset based on the amount of geometry this will add
		l_trackedOffset.indexOffset += element->mesh_creation_state.index_count;
		l_trackedOffset.vertexOffset += element->mesh_creation_state.vertex_count;
	}

	// Update sizes for the model data
	arModelData* modeldata = m_renderable->GetModelData();
	if (l_trackedOffset.indexOffset > m_renderableStreamedIndexStorageSize)
	{
		delete[] modeldata->indices;
		m_renderableStreamedIndexStorageSize = l_trackedOffset.indexOffset;
		modeldata->indices = new uint16_t[m_renderableStreamedIndexStorageSize];
	}
	if (l_trackedOffset.vertexOffset > m_renderableStreamedVertexStorageSize)
	{
		delete[] modeldata->position;
		delete[] modeldata->normal;
		delete[] modeldata->color;
		delete[] modeldata->texcoord0;
		delete[] modeldata->texcoord1;
		m_renderableStreamedVertexStorageSize = l_trackedOffset.indexOffset;
		modeldata->position = new Vector3f[m_renderableStreamedVertexStorageSize];
		modeldata->normal = new Vector3f[m_renderableStreamedVertexStorageSize];
		modeldata->color = new Vector4f[m_renderableStreamedVertexStorageSize];
		modeldata->texcoord0 = new Vector3f[m_renderableStreamedVertexStorageSize];
		modeldata->texcoord1 = new Vector3f[m_renderableStreamedVertexStorageSize];
	}
	modeldata->indexNum = l_trackedOffset.indexOffset;
	modeldata->vertexNum = l_trackedOffset.vertexOffset;

	// Copy the meshes for each mesh into the correct part of the final mesh
	std::vector<core::jobs::JobId> l_jobsToWaitOn (m_elements.size());
	for (uint32_t elementIndex = 0; elementIndex < m_elements.size(); ++elementIndex)
	{
		// Skip in-progress meshes
		if (m_elements[elementIndex]->mesh_creation_state.building_mesh)
		{
			continue;
		}

		l_jobsToWaitOn[elementIndex] = core::jobs::System::Current::AddJobRequest([this, elementIndex, &modeldata, &l_meshOffsets](void)
		{
			const Element* element = m_elements[elementIndex];
			const MeshOffsets& target_offset = l_meshOffsets[elementIndex];
			
			// Copy the indicies over
			std::copy(element->mesh_creation_state.mesh_data.indices, 
					  element->mesh_creation_state.mesh_data.indices + element->mesh_creation_state.index_count,
					  modeldata->indices + target_offset.indexOffset);

			// Update the indicies for this element.
			for (uint32_t indicieIndex = 0; indicieIndex < element->mesh_creation_state.index_count; ++indicieIndex)
			{
				*(modeldata->indices + target_offset.indexOffset + indicieIndex) += target_offset.vertexOffset;
			}

			// Copy the vertices over.
			std::copy(element->mesh_creation_state.mesh_data.position, 
					  element->mesh_creation_state.mesh_data.position + element->mesh_creation_state.vertex_count,
					  modeldata->position + target_offset.vertexOffset);
			std::copy(element->mesh_creation_state.mesh_data.texcoord0, 
					  element->mesh_creation_state.mesh_data.texcoord0 + element->mesh_creation_state.vertex_count,
					  modeldata->texcoord0 + target_offset.vertexOffset);
			std::copy(element->mesh_creation_state.mesh_data.texcoord1, 
					  element->mesh_creation_state.mesh_data.texcoord1 + element->mesh_creation_state.vertex_count,
					  modeldata->texcoord1 + target_offset.vertexOffset);
			std::copy(element->mesh_creation_state.mesh_data.color, 
					  element->mesh_creation_state.mesh_data.color + element->mesh_creation_state.vertex_count,
					  modeldata->color + target_offset.vertexOffset);
			std::copy(element->mesh_creation_state.mesh_data.normal, 
					  element->mesh_creation_state.mesh_data.normal + element->mesh_creation_state.vertex_count,
					  modeldata->normal + target_offset.vertexOffset);
		});
	}
	// Wait for all the mesh copying to finish
	for (const core::jobs::JobId jobId : l_jobsToWaitOn)
	{
		core::jobs::System::Current::WaitForJob(jobId);
	}
}

//	PostStepSynchronus() : Synchronus post-render
void ui::eventide::UserInterface::PostStepSynchronus ( void )
{
	// Update the material's textures
	auto passAccessor = m_renderable->PassAccess(0);
	for (uint32_t textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
	{
		passAccessor.setTexture((rrTextureSlot)(rrTextureSlot::TEX_SLOT0 + textureIndex), m_textures[textureIndex]);
	}
}

//	PreStepSynchronus() : Synchronous pre-render
void ui::eventide::UserInterface::PreStepSynchronus ( void )
{
	m_renderable->StreamLockModelData();
}