#include "UserInterface.h"

#include "core/debug/Console.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "core-ext/threads/Jobs.h"

#include "engine-common/dusk/Dusk.h"
#include "engine-common/dawn/DawnGUI.h"

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
	: CGameBehavior()
	, RrLogicObject()
	, m_duskUI(duskUI)
	, m_dawnUI(dawnUI)
{
	if (l_ActiveManager == NULL)
	{
		l_ActiveManager = this;
	}

	if (m_duskUI) m_duskUI->AddReference();
	if (m_dawnUI) m_dawnUI->AddReference();

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
	uiPass.setVertexSpecificationByCommonList(t_vspec, 5);
	uiPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	uiPass.m_alphaMode = renderer::kAlphaModeTranslucent;
	uiPass.m_blendMode = renderer::kHLBlendModeNormal;
	uiPass.m_cullMode = gpu::kCullModeNone;
	m_renderable->PassInitWithInput(0, &uiPass);

	m_blackTexture = RrTexture::Load(renderer::kTextureBlack);
}

ui::eventide::UserInterface::~UserInterface ( void )
{
	delete m_renderable;

	if (m_duskUI) m_duskUI->RemoveReference();
	if (m_dawnUI) m_dawnUI->RemoveReference();

	m_shuttingDown = true;

	// Free all allocated elements now
	for (ui::eventide::Element*& element : m_elements)
	{
		delete_safe(element);
	}
	m_elements.clear();

	m_blackTexture->RemoveReference();
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
	else
	{
		auto element_iter = std::find(m_elements.begin(), m_elements.end(), element);
		ARCORE_ASSERT(element_iter != m_elements.end());
		*element_iter = nullptr;
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
	for (ui::eventide::Element* element : m_elements) // This needs to be able to delay mesh regen until this is done.
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
		element->m_bboxDirty = false;
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
	if (RrCamera::activeCamera != NULL && (m_duskUI == NULL || !m_duskUI->IsMouseInside()))
	{
		const Vector2f mouseScreenPosition (Input::MouseX() / Screen::Info.width, Input::MouseY() / Screen::Info.height);
		const Ray mouseRay = Ray(
			RrCamera::activeCamera->transform.position,
			RrCamera::activeCamera->ScreenToWorldDir(mouseScreenPosition)
			);
		
		// find element with the shortest distance to the camera. that's the one with the mouse focus

		float minMouseHitDistance = INFINITY;
		Element* minMouseHitElement = m_currentMouseLocked ? m_currentMouseLockedElement : NULL;

		if (!m_currentMouseLocked)
		{
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
		}
		else if (m_currentMouseLockedElement != NULL)
		{
			ui::eventide::Element* element = m_currentMouseLockedElement;
			bool hasValidHit = false;

			// Attempt to do collision with the locked element
			const ui::eventide::Element::MouseInteract elementInteractStyle = element->GetMouseInteract();

			if (elementInteractStyle == ui::eventide::Element::MouseInteract::kBlocking
				|| elementInteractStyle == ui::eventide::Element::MouseInteract::kCapturing)
			{
				float mouseHitDistance = 0.0F;
				if (element->GetBBoxAbsolute().Raycast(mouseRay, mouseHitDistance))
				{
					minMouseHitDistance = mouseHitDistance;
					hasValidHit = true;
				}
			}

			if (!hasValidHit)
			{
				// Make a plane parallel to the camera at the object's center, and cast against that.
				core::math::Plane testPlane (element->GetBBox().GetCenterPoint(), RrCamera::activeCamera->transform.rotation * Vector3f::forward);

				float mouseHitDistance = 0.0F;
				if (testPlane.Raycast(mouseRay, mouseHitDistance))
				{
					minMouseHitDistance = mouseHitDistance;
					hasValidHit = true;
				}
			}

			// If we don't have a valid hit at this point, the element has gone behind the camera. This is a bad state to be in.
			ARCORE_ASSERT(hasValidHit);
		}

		// Save hit position:
		m_mouseLastHitPosition = mouseRay.pos + mouseRay.dir * minMouseHitDistance;

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
		else if (m_currentMouseOverElement != NULL)
		{
			// Staying inside the current element:
			for (int mouseButton = 0; mouseButton < 4; ++mouseButton)
			{
				if (Input::MouseDown(mouseButton))
				{
					event.type = ui::eventide::Element::EventMouse::Type::kClicked;
					event.button = mouseButton;
					m_currentMouseOverElement->OnEventMouse(event);

					m_mouseDragReference[mouseButton] = event.position_world;
				}
				if (Input::Mouse(mouseButton))
				{
					Vector3f mouseDelta = event.position_world - m_mouseDragReference[mouseButton];
					if (mouseDelta.sqrMagnitude() > FLOAT_PRECISION)
					{
						event.type = ui::eventide::Element::EventMouse::Type::kDragged;
						event.button = mouseButton;
						event.velocity_world = mouseDelta;
						m_currentMouseOverElement->OnEventMouse(event);

						m_mouseDragReference[mouseButton] = event.position_world;
					}
				}
				if (Input::MouseUp(mouseButton))
				{
					event.type = ui::eventide::Element::EventMouse::Type::kReleased;
					event.button = mouseButton;
					m_currentMouseOverElement->OnEventMouse(event);
				}
			}
			
		}
	}

	// Do other events:
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

		const ui::eventide::Element::FrameUpdate frameUpdateStyle = element->GetFrameUpdate();
		if (frameUpdateStyle == ui::eventide::Element::FrameUpdate::kPerFrame)
		{
			element->OnGameFrameUpdate( ui::eventide::Element::GameFrameUpdateInput() );
		}
	}
}

bool ui::eventide::UserInterface::IsMouseInside ( void )
{
	return m_currentMouseOverElement != NULL
		|| (m_duskUI != NULL && m_duskUI->IsMouseInside())
		|| (m_dawnUI != NULL && m_dawnUI->IsMouseInside());
}

void ui::eventide::UserInterface::LockMouse ( void )
{
	m_currentMouseLockedElement = m_currentMouseOverElement;
	m_currentMouseLocked = true;
}

void ui::eventide::UserInterface::UnlockMouse ( void )
{
	m_currentMouseLocked = false;
}

ui::eventide::Element* ui::eventide::UserInterface::GetMouseHit ( void ) const
{
	return (m_currentMouseLocked && m_currentMouseLockedElement != NULL) ? m_currentMouseLockedElement : m_currentMouseOverElement;
}

const Vector3f& ui::eventide::UserInterface::GetMousePosition ( void ) const
{
	return m_mouseLastHitPosition;
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
		texture_location = (--m_textures.end());
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
		texture_location = (--m_textures.end());
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
	std::vector<Element*> l_renderedElements = m_elements;
	
	// Rebuild all the meshes as needed
	std::vector<core::jobs::JobId> l_rebuildJobs;
	for (uint32_t elementIndex = 0; elementIndex < l_renderedElements.size(); ++elementIndex)
	{
		Element* element = l_renderedElements[elementIndex];

		// Add meshes that need new meshes to the job list
		if (element->mesh_creation_state.rebuild_requested)
		{
			l_rebuildJobs.push_back(core::jobs::System::Current::AddJobRequest([element](void)
			{
				// Should be safe, as bbox dirty is not cleared until completely done writing.
				element->mesh_creation_state.rebuild_requested = element->m_bboxDirty;
				element->RebuildMesh();
			}));
		}
	}
	// Wait for all the mesh generation to finish
	for (const core::jobs::JobId jobId : l_rebuildJobs)
	{
		core::jobs::System::Current::WaitForJob(jobId);
	}

	std::vector<MeshOffsets> l_meshOffsets (l_renderedElements.size());
	MeshOffsets l_trackedOffset = {0, 0};

	// Generate the offsets for each mesh
	for (uint32_t elementIndex = 0; elementIndex < l_renderedElements.size(); ++elementIndex)
	{
		const Element* element = l_renderedElements[elementIndex];
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
	std::vector<core::jobs::JobId> l_jobsToWaitOn (l_renderedElements.size());
	for (uint32_t elementIndex = 0; elementIndex < l_renderedElements.size(); ++elementIndex)
	{
		// Skip in-progress meshes
		if (l_renderedElements[elementIndex]->mesh_creation_state.building_mesh)
		{
			continue;
		}

		l_jobsToWaitOn[elementIndex] = core::jobs::System::Current::AddJobRequest([this, &l_renderedElements, elementIndex, &modeldata, &l_meshOffsets](void)
		{
			const Element* element = l_renderedElements[elementIndex];
			const MeshOffsets& target_offset = l_meshOffsets[elementIndex];
			
			// Copy the indicies over
			std::copy(element->mesh_creation_state.mesh_data.indices, 
					  element->mesh_creation_state.mesh_data.indices + element->mesh_creation_state.index_count,
					  modeldata->indices + target_offset.indexOffset);

			// Update the indicies for this element.
			for (uint32_t indicieIndex = 0; indicieIndex < element->mesh_creation_state.index_count; ++indicieIndex)
			{
				auto& current_index = *(modeldata->indices + target_offset.indexOffset + indicieIndex);
				current_index = (current_index != 0xFFFF) ? (current_index + target_offset.vertexOffset) : 0xFFFF;
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
	for (int32_t textureIndex = 0; textureIndex < m_textures.size(); ++textureIndex)
	{
		passAccessor.setTexture((rrTextureSlot)(rrTextureSlot::TEX_SLOT0 + textureIndex), m_textures[textureIndex] );
	}
	for (size_t textureIndex = m_textures.size(); textureIndex < kPass_MaxTextureSlots; ++textureIndex)
	{
		passAccessor.setTexture((rrTextureSlot)(rrTextureSlot::TEX_SLOT0 + textureIndex), m_blackTexture);
	}
}

//	PreStepSynchronus() : Synchronous pre-render
void ui::eventide::UserInterface::PreStepSynchronus ( void )
{
	m_renderable->StreamLockModelData();
}