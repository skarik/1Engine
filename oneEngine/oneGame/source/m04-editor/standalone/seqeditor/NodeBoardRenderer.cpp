#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"
#include "./NodeBoardState.h"
#include "./Constants.h"
#include "./SequenceEditor.h"
#include "core-ext/containers/arStringEnum.h"

#include "NodeBoardRenderer.h"

m04::editor::sequence::NodeRenderer::NodeRenderer (m04::editor::sequence::NodeBoardState* in_nbs, m04::editor::sequence::BoardNode* in_node, ui::eventide::UserInterface* ui)
	: ui::eventide::elements::Button(ui)
	, m04::editor::sequence::INodeDisplay(in_node)
	, m_board(in_nbs)
	, m_halfsizeOnBoard(Vector3f(80, 40, 5))
	, m_margins(Vector3f(10, 5, 5))
	, m_padding(Vector3f(10, 5, 5))
	, m_connectionHalfsize(Vector3f(10, 10, 10))
{
	m_mouseInteract = MouseInteract::kCapturing;
	m_frameUpdate = FrameUpdate::kPerFrame;

	m_uiElementsTexture = LoadTexture(m04::editor::sequence::gFilenameGUIElementTexture);

	ARCORE_ASSERT(in_node->sequenceInfo != NULL); // Must ensure node exists
	ARCORE_ASSERT(in_node->sequenceInfo->view != NULL); // Must ensure view is valid

	// Set position & bbox
	SetBBox(core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard));

	// Set up the style infos
	m_bbox_flow_input = core::math::BoundingBox(
		Rotator(), 
		Vector3f(-m_halfsizeOnBoard.x, 0.0F - m_connectionHalfsize.y - m_margins.y, 0), 
		Vector3f(m_connectionHalfsize.x, m_connectionHalfsize.y, 4)
		);
	m_bbox_flow_output = core::math::BoundingBox(
		Rotator(),
		Vector3f(m_halfsizeOnBoard.x, 0.0F - m_connectionHalfsize.y - m_margins.y, 0),
		Vector3f(m_connectionHalfsize.x, m_connectionHalfsize.y, 4)
		);

	UpdateCachedVisualInfo();
	UpdateNextNode();

	// Set up the properties and extra
	m_propertyState.resize(node->sequenceInfo->view->PropertyList().size());
}

m04::editor::sequence::NodeRenderer::~NodeRenderer ( void )
{
	ReleaseTexture(m_uiElementsTexture);
}

void m04::editor::sequence::NodeRenderer::UpdateCachedVisualInfo ( void )
{
	// Cache all needed info
	std::string l_displayNameModified = ISequenceNodeClassInfo::GetInfo(node->sequenceInfo->view->classname)->m_displayname;
	{
		size_t l_lastUnderscore = l_displayNameModified.find_last_of('_');
		if (l_lastUnderscore != string::npos)
		{
			l_displayNameModified = l_displayNameModified.substr(l_lastUnderscore + 1);
		}
	}
	m_display_text = l_displayNameModified.c_str();
	m_guid_text = node->guid.toString().c_str();
}

void m04::editor::sequence::NodeRenderer::OnEventMouse ( const EventMouse& mouse_event )
{
	if (mouse_event.type == EventMouse::Type::kDragged)
	{
		if (m_dragging)
		{
			core::math::BoundingBox bbox = m_draggingStart;
			bbox.m_M.translate(mouse_event.velocity_world);
			bbox.m_MInverse = bbox.m_M.inverse();
			m_draggingStart = bbox;

			// Update snapping
			auto& grid_state = m_board->GetEditor()->GetGridState();
			Vector3f bboxPosition = bbox.m_M.getTranslation();
			if (grid_state.snapX)
			{
				bboxPosition.x = math::round((bboxPosition.x - m_halfsizeOnBoard.x) / grid_state.gridSize) * grid_state.gridSize + m_halfsizeOnBoard.x;
			}
			if (grid_state.snapY)
			{
				bboxPosition.y = math::round((bboxPosition.y + m_halfsizeOnBoard.y) / grid_state.gridSize) * grid_state.gridSize - m_halfsizeOnBoard.y;
			}
			bbox.m_M.setTranslation(bboxPosition);

			node->position = bbox.GetCenterPoint() - m_halfsizeOnBoard;

			SetBBox( bbox );
			RequestUpdateMesh();
		}
	}
	else if (mouse_event.type == EventMouse::Type::kClicked || mouse_event.type == EventMouse::Type::kReleased)
	{
		if (mouse_event.type == EventMouse::Type::kClicked
			&& mouse_event.button == core::kMBLeft)
		{
			OnClicked(mouse_event);
		}
		else if (mouse_event.type == EventMouse::Type::kReleased)
		{
			OnReleased(mouse_event);
		}


		auto& nodeProperties = node->sequenceInfo->view->PropertyList();
		for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
		{
			auto& nodeProperty = nodeProperties[nodePropertyIndex];
			
			core::math::BoundingBox l_bbox_property = GetBboxProperty<PropertyComponent::All>(nodePropertyIndex);
			core::math::BoundingBox l_bbox_property_key = GetBboxProperty<PropertyComponent::Key>(nodePropertyIndex);

			if (l_bbox_property.IsPointInBox(mouse_event.position_world))
			{
				m_propertyState[nodePropertyIndex].m_hovered = true;

				if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kBoolean)
				{
					if (mouse_event.type == EventMouse::Type::kClicked
						&& mouse_event.button == core::kMBLeft)
					{
						if (l_bbox_property_key.IsPointInBox(mouse_event.position_world))
						{
							node->sequenceInfo->view->SetProperty(nodeProperty.identifier, !node->sequenceInfo->view->GetPropertyAsBool(nodeProperty.identifier));
						}
					}
				}
				else if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kFloat)
				{
					if (mouse_event.type == EventMouse::Type::kClicked
						&& mouse_event.button == core::kMBLeft)
					{
						m_propertyState[nodePropertyIndex].m_editing = true;
					}
				}
				else if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kScriptText)
				{
					if (mouse_event.type == EventMouse::Type::kClicked
						&& mouse_event.button == core::kMBLeft)
					{
						m_propertyState[nodePropertyIndex].m_editing = true;
					}
				}
				else if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kEnumtypeDropdown
					|| nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kScriptCharacter)
				{
					if (mouse_event.type == EventMouse::Type::kClicked
						&& mouse_event.button == core::kMBLeft)
					{
						// Now, render the current setting (find a matching enum first)
						const auto& editorEnums = m_board->GetEditor()->GetEnums();
						arstring128 enumDefinitionName;
						arStringEnumDefinition* enumDefinition = NULL;

						std::string identifierLower = nodeProperty.identifier;
						core::utils::string::ToLower(identifierLower);

						if (editorEnums.find(identifierLower.c_str()) != editorEnums.end())
						{
							enumDefinitionName = identifierLower.c_str();
						}
						else
						{
							// Take display name and remove the spaces
							std::string moddedDisplayName = nodeProperty.label;
							moddedDisplayName.erase(std::remove(moddedDisplayName.begin(), moddedDisplayName.end(), ' '));
							core::utils::string::ToLower(moddedDisplayName);

							if (editorEnums.find(moddedDisplayName.c_str()) != editorEnums.end())
							{
								enumDefinitionName = moddedDisplayName.c_str();
							}
						}

						ARCORE_ASSERT(enumDefinitionName.length() > 0);
						enumDefinition = editorEnums.find(enumDefinitionName)->second;

						if (enumDefinition != NULL)
						{
							// find matching enum name
							int32_t currentEnumIndex = -1;
							const char* str = node->sequenceInfo->view->GetPropertyAsString(nodeProperty.identifier);
							if (str == NULL || strlen(str) == 0)
							{
								currentEnumIndex = 0;
							}
							else
							{
								auto enumValue = enumDefinition->CreateValue(str);
								if (enumValue.IsValid())
								{
									currentEnumIndex = enumValue.GetEnumIndex();
								}
								else
								{
									currentEnumIndex = 0;
								}
							}

							// scroll thru values
							currentEnumIndex++;
							auto enumValue = enumDefinition->CreateValueFromIndex(currentEnumIndex);
							if (enumValue.IsValid())
							{
								node->sequenceInfo->view->SetProperty(nodeProperty.identifier, enumValue.GetName());
							}
							else
							{
								node->sequenceInfo->view->SetProperty(nodeProperty.identifier, enumDefinition->CreateValueFromIndex(0).GetName());
							}
						}
					}
				}
			}
			else
			{
				m_propertyState[nodePropertyIndex].m_hovered = false;
				m_propertyState[nodePropertyIndex].m_editing = false;
			}
		}


		Button::OnEventMouse(mouse_event);
	}
}

void m04::editor::sequence::NodeRenderer::OnClicked ( const EventMouse& mouse_event )
{
	const bool bDoDraggingAsFallback = (!m_draggingInfo.active);

	bool bAllowCapture = true; // Flag if we captured something already? Should be used to skip and fallback.

	core::math::BoundingBox l_bbox_flow_input = GetBboxFlowInput();
	if (bAllowCapture
		&& l_bbox_flow_input.IsPointInBox(mouse_event.position_world))
	{
		bAllowCapture = false;
		m_draggingInfo = {true, DragState::Target::kFlowInput, 0};
		m_mouseInteract = MouseInteract::kCapturingCatchAll;
	}
	else if (bAllowCapture)
	{
		for (uint32_t flowOutputIndex = 0; flowOutputIndex < node->sequenceInfo->view->Flow().outputCount; ++flowOutputIndex)
		{
			core::math::BoundingBox l_bbox_flow_output = GetBboxFlowOutput(flowOutputIndex);
			if (l_bbox_flow_output.IsPointInBox(mouse_event.position_world))
			{
				bAllowCapture = false;
				m_draggingInfo = {true, DragState::Target::kFlowOutput, flowOutputIndex};
				m_mouseInteract = MouseInteract::kCapturingCatchAll;

				// No more check
				break;
			}
		}
	}

	//core::math::BoundingBox l_bbox_flow_input = GetBboxFlowInput();
	if (bAllowCapture
		&& false)
	{

	}
	else if (bAllowCapture && bDoDraggingAsFallback)
	{
		m_dragging = true;
		m_draggingStart = GetBBox();
		m_ui->LockMouse();
	}
}
void m04::editor::sequence::NodeRenderer::OnReleased ( const EventMouse& mouse_event )
{
	ARCORE_ASSERT(!m_draggingInfo.active || !m_dragging);

	if (m_draggingInfo.active)
	{
		/*
		if (GetMouseInside())
		{
		}*/

		// Check if hit another node
		if (m_draggingInfo.target == DragState::Target::kFlowInput
			|| m_draggingInfo.target == DragState::Target::kFlowOutput
			|| m_draggingInfo.target == DragState::Target::kFlowSync
			|| m_draggingInfo.target == DragState::Target::kLogicInput
			|| m_draggingInfo.target == DragState::Target::kLogicOutput)
		{
			ui::eventide::Element* hitElement = m_ui->GetMouseHit();
			if (hitElement != NULL)
			{
				NodeRenderer* hitElementAsNodeRenderer = dynamic_cast<NodeRenderer*>(hitElement);
				if (hitElementAsNodeRenderer != NULL)
				{
					if (m_draggingInfo.target == DragState::Target::kFlowInput)
					{
						hitElementAsNodeRenderer->GetBoardNode()->sequenceInfo->next = node->sequenceInfo;
						hitElementAsNodeRenderer->UpdateNextNode();
					}
					else if (m_draggingInfo.target == DragState::Target::kFlowOutput)
					{
						node->sequenceInfo->next = hitElementAsNodeRenderer->GetBoardNode()->sequenceInfo;
						m_next = hitElementAsNodeRenderer;
					}
				}
			}
		}

		m_draggingInfo.active = false;
		m_mouseInteract = MouseInteract::kCapturing;
	}
	else if (m_dragging)
	{
		m_dragging = false;
		m_ui->UnlockMouse();
	}
}

void m04::editor::sequence::NodeRenderer::BuildMesh ( void )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;
	ParamsForPath pathParams;

	const Vector3f nodeExtents = GetBBoxAbsolute().GetExtents();

	cubeParams = {};
	cubeParams.box = core::math::Cubic::ConstructFromBBox(GetBBoxAbsolute());
	cubeParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	cubeParams.texture = NULL;
	cubeParams.color = DefaultStyler.box.defaultColor
		.Lerp(DefaultStyler.box.hoverColor, m_hoverGlowValue)
		.Lerp(DefaultStyler.box.activeColor, Styler::PulseFade(m_activateGlowPulse));
	//cubeParams.wireframe = true; // TODO
	buildCube(cubeParams);

	Vector3f nodeTopLeft = GetBBoxAbsolute().GetCenterPoint()
		- Vector3f(nodeExtents.x, -nodeExtents.y, nodeExtents.z)
		+ Vector3f(0, 0, nodeExtents.z * 2.0F + 1.0F);

	// Show the view's name
	textParams = {};
	textParams.string = m_display_text;
	textParams.font_texture = &m_fontTexture;
	textParams.position = nodeTopLeft + Vector3f(m_margins.x, -ui::eventide::DefaultStyler.text.headingSize - m_margins.y, 0);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = ui::eventide::DefaultStyler.text.headingSize;
	textParams.color = DefaultStyler.text.headingColor;
	buildText(textParams);

	// Show the node's GUID at the top
	textParams = {};
	textParams.string = m_guid_text.c_str();
	textParams.font_texture = &m_fontTexture;
	textParams.position = nodeTopLeft + Vector3f(m_halfsizeOnBoard.x * 2.0F - m_margins.x, -ui::eventide::DefaultStyler.text.buttonSize - m_margins.y, 0);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = ui::eventide::DefaultStyler.text.buttonSize;
	textParams.alignment = AlignHorizontal::kRight;
	textParams.color = DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.75F);
	buildText(textParams);

	// Flow input
	if (node->sequenceInfo->view->Flow().inputCount != 0)
	{
		core::math::BoundingBox l_bbox_flow_input = GetBboxFlowInput();

		quadParams = {};
		quadParams.position = l_bbox_flow_input.GetCenterPoint();
		quadParams.size = Vector2f(l_bbox_flow_input.GetExtents().x, l_bbox_flow_input.GetExtents().y);
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_uiElementsTexture;
		quadParams.color = DefaultStyler.box.defaultColor;
		buildQuad(quadParams);
	}

	// Flow output
	for (uint32_t flowOutputIndex = 0; flowOutputIndex < node->sequenceInfo->view->Flow().outputCount; ++flowOutputIndex)
	{
		core::math::BoundingBox l_bbox_flow_output = GetBboxFlowOutput(flowOutputIndex);

		quadParams = {};
		quadParams.position = l_bbox_flow_output.GetCenterPoint();
		quadParams.size = Vector2f(l_bbox_flow_output.GetExtents().x, l_bbox_flow_output.GetExtents().y);
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_uiElementsTexture;
		quadParams.color = DefaultStyler.box.defaultColor;
		buildQuad(quadParams);
	}

	// Draw drag nonsense
	if (m_draggingInfo.active)
	{
		cubeParams = {};
		cubeParams.box = core::math::Cubic::ConstructCenterExtents(m_ui->GetMousePosition(), Vector3f(5, 5, 5));
		cubeParams.color = Color(1, 0, 0, 1);
		cubeParams.texture = NULL;
		buildCube(cubeParams);
	}

	// Draw the flow output
	if (m_next == NULL)
	{
		;	
	}
	else
	{
		NodeRenderer* next_noderenderer = (NodeRenderer*)m_next;

		cubeParams = {};
		cubeParams.box = core::math::Cubic::ConstructCenterExtents((GetBboxFlowOutput(0).GetCenterPoint() + next_noderenderer->GetBboxFlowInput().GetCenterPoint()) * 0.5F, Vector3f(5, 5, 5));
		cubeParams.color = Color(1, 0, 0, 1);
		cubeParams.texture = NULL;
		buildCube(cubeParams);

		pathParams = {};
		Vector3f pointArray[] = {
			Vector3f(GetBboxFlowOutput(0).GetCenterPoint()),
			Vector3f((GetBboxFlowOutput(0).GetCenterPoint() + next_noderenderer->GetBboxFlowInput().GetCenterPoint()) * 0.5F),
			Vector3f(next_noderenderer->GetBboxFlowInput().GetCenterPoint()),
		};
		pathParams.points = pointArray;
		pathParams.pointCount = 3;
		buildPath(pathParams);
	}
	// TODO: when dragging the output to the node

	// Draw all the options
	auto& nodeProperties = node->sequenceInfo->view->PropertyList();
	Vector3f l_bboxPenPosition = Vector3f(0.0F, -ui::eventide::DefaultStyler.text.headingSize - m_margins.y, 0.0F);
	//for (auto& nodeProperty : nodeProperties)
	for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
	{
		auto& nodeProperty = nodeProperties[nodePropertyIndex];

		switch (nodeProperty.renderstyle)
		{
		case m04::editor::PropertyRenderStyle::kBoolean:
			BuildMeshPropertyBoolean(nodeTopLeft, nodeProperty, nodePropertyIndex, l_bboxPenPosition);
			break;

		case m04::editor::PropertyRenderStyle::kFloat:
			BuildMeshPropertyFloat(nodeTopLeft, nodeProperty, nodePropertyIndex, l_bboxPenPosition);
			break;

		case m04::editor::PropertyRenderStyle::kScriptText:
			BuildMeshPropertyScriptText(nodeTopLeft, nodeProperty, nodePropertyIndex, l_bboxPenPosition);
			break;

		case m04::editor::PropertyRenderStyle::kScriptCharacter:
		case m04::editor::PropertyRenderStyle::kEnumtypeDropdown:
			BuildMeshPropertyEnumDropdown(nodeTopLeft, nodeProperty, nodePropertyIndex, l_bboxPenPosition);
			break;
		}
	}
}

void m04::editor::sequence::NodeRenderer::BuildMeshPropertyBoolean ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	const core::math::BoundingBox bbox_all = GetBboxProperty<PropertyComponent::All>(in_propertyIndex);
	const core::math::BoundingBox bbox_key = GetBboxProperty<PropertyComponent::Key>(in_propertyIndex);

	textParams = {};
	textParams.string = in_property.label.c_str();
	textParams.font_texture = &m_fontTexture;
	//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, 0) + Vector3f(ui::eventide::DefaultStyler.text.headingSize + m_padding.x, 0, 0);
	textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState[in_propertyIndex].m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);

	quadParams = {};
	quadParams.position = bbox_key.GetCenterPoint();
	quadParams.size = Vector3f(1, 1, 1) * (bbox_key.GetExtents().y);
	quadParams.color = Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	if (node->sequenceInfo->view->GetPropertyAsBool(in_property.identifier))
	{
		quadParams = {};
		quadParams.position = bbox_key.GetCenterPoint() + Vector3f(0, 0, 2);
		quadParams.size = Vector3f(1, 1, 1) * (bbox_key.GetExtents().y - m_padding.x * 0.5F);
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_uiElementsTexture;
		quadParams.color = Color(1, 1, 1, 1);
		buildQuad(quadParams);
	}

	// Push down pen
	inout_penPosition.y -= ui::eventide::DefaultStyler.text.buttonSize + 5;
}

void m04::editor::sequence::NodeRenderer::BuildMeshPropertyFloat ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	const core::math::BoundingBox bbox_all = GetBboxProperty<PropertyComponent::All>(in_propertyIndex);
	const core::math::BoundingBox bbox_key = GetBboxProperty<PropertyComponent::Key>(in_propertyIndex);

	textParams = {};
	textParams.string = in_property.label.c_str();
	textParams.font_texture = &m_fontTexture;
	//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z);
	textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState[in_propertyIndex].m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);

	quadParams = {};
	quadParams.position = bbox_key.GetCenterPoint();
	quadParams.size = bbox_key.GetExtents();
	quadParams.color = m_propertyState[in_propertyIndex].m_hovered ? Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F) : DefaultStyler.box.defaultColor;
	buildQuad(quadParams);

	const char* str = NULL;
	std::string str_tempBuffer;
	if (m_propertyState[in_propertyIndex].m_editing)
	{
		str = node->sequenceInfo->view->GetPropertyAsString(in_property.identifier);
	}
	else
	{
		const float currentValue = node->sequenceInfo->view->GetPropertyAsFloat(in_property.identifier);
		str_tempBuffer = std::to_string(currentValue);
		str_tempBuffer.erase(str_tempBuffer.find_last_not_of('0') + 1, std::string::npos);
		str_tempBuffer.erase(str_tempBuffer.find_last_not_of('.') + 1, std::string::npos); // See the node conversion GetPropertyAsFloat/GetPropertyAsString
		str = str_tempBuffer.c_str();
	}

	if (str)
	{
		std::string l_drawString = str;
		if (m_propertyState[in_propertyIndex].m_editing)
		{
			if (fmod(Time::currentTime, 1.0F) < 0.5F)
			{
				l_drawString += '|';
			}
		}

		textParams = {};
		textParams.string = l_drawString.c_str();
		textParams.font_texture = &m_fontTexture;
		textParams.position = bbox_key.GetCenterPoint() - Vector3f(bbox_key.GetExtents().x, bbox_key.GetExtents().y, 0) + Vector3f(0, 0, 0.2F);
		textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = Color(1, 1, 1, 1);
		buildText(textParams);
	}

	// Push down pen
	inout_penPosition.y -= ui::eventide::DefaultStyler.text.buttonSize + 5;
}

void m04::editor::sequence::NodeRenderer::BuildMeshPropertyScriptText ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	const core::math::BoundingBox bbox_all = GetBboxProperty<PropertyComponent::All>(in_propertyIndex);
	const core::math::BoundingBox bbox_key = GetBboxProperty<PropertyComponent::Key>(in_propertyIndex);

	quadParams = {};
	quadParams.position = bbox_all.GetCenterPoint();
	quadParams.size = bbox_all.GetExtents();
	quadParams.color = m_propertyState[in_propertyIndex].m_hovered ? Color(1,1,1,1) : Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	textParams = {};
	textParams.string = in_property.label.c_str();
	textParams.font_texture = &m_fontTexture;
	//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z);
	textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState[in_propertyIndex].m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);

	const char* str = node->sequenceInfo->view->GetPropertyAsString(in_property.identifier);
	if (str)
	{
		std::string l_drawString = str;
		if (m_propertyState[in_propertyIndex].m_editing)
		{
			if (fmod(Time::currentTime, 1.0F) < 0.5F)
			{
				l_drawString += '|';
			}
		}

		textParams = {};
		textParams.string = l_drawString.c_str();
		textParams.font_texture = &m_fontTexture;
		//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z) + Vector3f(0, 0, 0.1F);
		textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, 0) + Vector3f(0, 0, 0.2F);
		textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = Color(1, 0, 0, 1);
		buildText(textParams);
	}

	// Push down pen
	inout_penPosition.y -= ui::eventide::DefaultStyler.text.buttonSize + 5;
}

void m04::editor::sequence::NodeRenderer::BuildMeshPropertyEnumDropdown ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	const core::math::BoundingBox bbox_all = GetBboxProperty<PropertyComponent::All>(in_propertyIndex);
	const core::math::BoundingBox bbox_key = GetBboxProperty<PropertyComponent::Key>(in_propertyIndex);

	textParams = {};
	textParams.string = in_property.label.c_str();
	textParams.font_texture = &m_fontTexture;
	textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState[in_propertyIndex].m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);

	/*quadParams = {};
	quadParams.position = bbox_key.GetCenterPoint();
	quadParams.size = Vector3f(1, 1, 1) * (bbox_key.GetExtents().y);
	quadParams.color = Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	if (node->sequenceInfo->view->GetPropertyAsBool(in_property.identifier))
	{
		quadParams = {};
		quadParams.position = bbox_key.GetCenterPoint() + Vector3f(0, 0, 2);
		quadParams.size = Vector3f(1, 1, 1) * (bbox_key.GetExtents().y - m_padding.x * 0.5F);
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_uiElementsTexture;
		quadParams.color = Color(1, 1, 1, 1);
		buildQuad(quadParams);
	}*/

	// TODO: so much of this can be moved to a separate class and cached (or at least just cached)

	// Now, render the current setting (find a matching enum first)
	const auto& editorEnums = m_board->GetEditor()->GetEnums();
	arstring128 enumDefinitionName;
	arStringEnumDefinition* enumDefinition = NULL;

	std::string identifierLower = in_property.identifier;
	core::utils::string::ToLower(identifierLower);

	if (editorEnums.find(identifierLower.c_str()) != editorEnums.end())
	{
		enumDefinitionName = identifierLower.c_str();
	}
	else
	{
		// Take display name and remove the spaces
		std::string moddedDisplayName = in_property.label;
		moddedDisplayName.erase(std::remove(moddedDisplayName.begin(), moddedDisplayName.end(), ' '));
		core::utils::string::ToLower(moddedDisplayName);

		if (editorEnums.find(moddedDisplayName.c_str()) != editorEnums.end())
		{
			enumDefinitionName = moddedDisplayName.c_str();
		}
	}

	ARCORE_ASSERT(enumDefinitionName.length() > 0);
	enumDefinition = editorEnums.find(enumDefinitionName)->second;

	if (enumDefinition != NULL)
	{
		// find matching enum name
		const char* str = node->sequenceInfo->view->GetPropertyAsString(in_property.identifier);
		if (str == NULL || strlen(str) == 0)
		{
			str = enumDefinition->GetEnumName(0); //todo
		}
		else
		{
			auto enumValue = enumDefinition->CreateValue(str);
			if (enumValue.IsValid())
			{
				str = enumValue.GetName();
			}
			else
			{
				str = enumDefinition->GetEnumName(0); //todo
			}
		}

		arstring256 camelCasedValue = core::utils::string::CamelCaseToReadable(str, strlen(str));
		camelCasedValue[0] = ::toupper(camelCasedValue[0]);

		textParams = {};
		textParams.string = camelCasedValue.c_str();
		textParams.font_texture = &m_fontTexture;
		textParams.position = bbox_key.GetCenterPoint() - Vector3f(bbox_key.GetExtents().x, bbox_key.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
		textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = m_propertyState[in_propertyIndex].m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
		buildText(textParams);
	}

	// Push down pen
	inout_penPosition.y -= ui::eventide::DefaultStyler.text.buttonSize + 5;
}

void m04::editor::sequence::NodeRenderer::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	core::math::BoundingBox bbox = GetBBox();

	Vector3f l_previousExtents = bbox.GetExtents();

	// Update size of the board
	m_halfsizeOnBoard.y = std::max(40.0F, (GetBboxOfAllProperties() + ui::eventide::DefaultStyler.text.headingSize + m_padding.y * 2.0F + m_margins.y * 2.0F) * 0.5F);
	// TODO: based on properties, update bbox
	node->position.y += (l_previousExtents.y - m_halfsizeOnBoard.y) * 2.0F;
	bbox = core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard);

	// Set bbox, push update for mesh
	SetBBox(bbox);
	// Request the mesh updates next frame
	RequestUpdateMesh();

	// Run the button updates
	Button::OnGameFrameUpdate(input_frame);

	// TODO: more organized
	// Update per-frame edits
	auto& nodeProperties = node->sequenceInfo->view->PropertyList();
	for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
	{
		auto& nodeProperty = nodeProperties[nodePropertyIndex];
		if (m_propertyState[nodePropertyIndex].m_editing)
		{
			if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kScriptText)
			{
				// Parse input and modify the underlying property
				std::string l_currentValue = node->sequenceInfo->view->GetPropertyAsString(nodeProperty.identifier);

				// Run the parse loop (see dusk/TextField.cpp)
				const auto& inputString = core::Input::FrameInputString();
				for (const auto& input : inputString)
				{
					if ( input && isprint(input) )
					{
						l_currentValue += input;
					}
					if ( input == core::kVkBackspace )
					{
						l_currentValue = l_currentValue.substr(0, l_currentValue.length() - 1);
					}
				}

				// Save back the edits
				node->sequenceInfo->view->SetProperty(nodeProperty.identifier, l_currentValue.c_str());
			}
			else if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kFloat)
			{
				// Parse input and modify the underlying property
				std::string l_currentValue = node->sequenceInfo->view->GetPropertyAsString(nodeProperty.identifier);

				// Run the parse loop (see dusk/TextField.cpp)
				const auto& inputString = core::Input::FrameInputString();
				for (const auto& input : inputString)
				{
					if ( input && isprint(input) && (isdigit(input) || input == '.') )
					{
						l_currentValue += input;
					}
					if ( input == core::kVkBackspace )
					{
						l_currentValue = l_currentValue.substr(0, l_currentValue.length() - 1);
					}
				}

				// Save back the edits
				node->sequenceInfo->view->SetProperty(nodeProperty.identifier, l_currentValue.c_str());
			}
		}
		else
		{
			if (nodeProperty.renderstyle == m04::editor::PropertyRenderStyle::kFloat)
			{
				// If the field is invalid, force the value of zero
				if (node->sequenceInfo->view->node->data[nodeProperty.identifier]->GetType() == osf::ValueType::kString)
				{
					std::string l_currentValue = node->sequenceInfo->view->GetPropertyAsString(nodeProperty.identifier);
					try
					{
						std::stof(l_currentValue); // Try the conversion. If it throws, the input is invalid.
					}
					catch (std::invalid_argument&)
					{
						node->sequenceInfo->view->SetProperty(nodeProperty.identifier, "0");
					}
				}
				// Forces the value to convert back to a floating point value.
				node->sequenceInfo->view->GetPropertyAsFloat(nodeProperty.identifier);
			}
		}
	}

	m_frameUpdate = FrameUpdate::kPerFrame;
}

void m04::editor::sequence::NodeRenderer::UpdateNextNode ( void )
{
	const NodeBoardState* nbs = m_board;

	// Find the node in the board with the matching pointer
	auto boardNodeIter = (this->node->sequenceInfo->next == NULL)
		? nbs->nodes.end()
		: std::find_if(nbs->nodes.begin(), nbs->nodes.end(), [this](BoardNode* board_node) { return this->node->sequenceInfo->next == board_node->sequenceInfo; });

	if (boardNodeIter != nbs->nodes.end())
	{
		m_next = (*boardNodeIter)->display;
	}
	else
	{	// Next node does not exist, update both this node and the next one
		m_next = NULL;
		node->sequenceInfo->next = NULL;
	}
}

core::math::BoundingBox m04::editor::sequence::NodeRenderer::GetBboxFlowInput ( void )
{
	core::math::BoundingBox l_bbox_flow_input = m_bbox_flow_input;
	l_bbox_flow_input.m_M.setTranslation(
		l_bbox_flow_input.m_M.getTranslation() 
		+ Vector3f(0, m_halfsizeOnBoard.y, m_bbox.GetExtents().z + 1)
		+ GetBBoxAbsolute().m_M.getTranslation());
	l_bbox_flow_input.m_MInverse = l_bbox_flow_input.m_M.inverse(); // TODO: optimize this

	return l_bbox_flow_input;
}
core::math::BoundingBox m04::editor::sequence::NodeRenderer::GetBboxFlowOutput ( const uint32_t output_index )
{
	core::math::BoundingBox l_bbox_flow_output = m_bbox_flow_output;
	l_bbox_flow_output.m_M.setTranslation(
		l_bbox_flow_output.m_M.getTranslation() 
		+ Vector3f(0, m_halfsizeOnBoard.y + output_index * m_bbox_flow_output.GetExtents().y * -2.0F, m_bbox.GetExtents().z + 1)
		+ GetBBoxAbsolute().m_M.getTranslation());
	l_bbox_flow_output.m_MInverse = l_bbox_flow_output.m_M.inverse(); // TODO: optimize this

	return l_bbox_flow_output;
}

template <m04::editor::sequence::NodeRenderer::PropertyComponent Part>
core::math::BoundingBox m04::editor::sequence::NodeRenderer::GetBboxProperty ( const uint32_t property_index )
{
	// TODO: optimize this
	const core::math::BoundingBox nodeBbox = GetBBoxAbsolute();

	// sum up all the offsets to the current position
	Vector3f offset;
	offset.y = -ui::eventide::DefaultStyler.text.headingSize - m_margins.y * 2.0F; // Start below the title.
	//offset.y -= ui::eventide::DefaultStyler.text.headingSize; // Push it down again.
	for (uint32_t checkPropertyIndex = 0; checkPropertyIndex < property_index; ++checkPropertyIndex)
	{
		auto nodeProperty = node->sequenceInfo->view->PropertyList()[checkPropertyIndex];
		switch (nodeProperty.renderstyle)
		{
		case m04::editor::PropertyRenderStyle::kBoolean:
			offset.y -= ui::eventide::DefaultStyler.text.buttonSize + m_padding.y;
			break;
		case m04::editor::PropertyRenderStyle::kScriptCharacter:
		case m04::editor::PropertyRenderStyle::kEnumtypeDropdown:
			offset.y -= ui::eventide::DefaultStyler.text.buttonSize + m_padding.y;
			break;
		case m04::editor::PropertyRenderStyle::kScriptText:
			{
				const Real l_boxWidth = nodeBbox.GetExtents().x * 2.0F - m_padding.x * 2.0F;
				const char* l_scriptString = node->sequenceInfo->view->GetPropertyAsString(nodeProperty.identifier);
				ParamsForText textParams;
				textParams = {};
				textParams.string = l_scriptString;
				textParams.font_texture = &m_fontTexture;
				textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
				const int l_numberOfLines = std::max(1, (int)(predictText(textParams).x / l_boxWidth) + 1);

				offset.y -= ui::eventide::DefaultStyler.text.headingSize * l_numberOfLines + m_padding.y;
			}
			break;
		}
	}

	// Apply the offset so we're working against the upper-left corner
	offset.x -= nodeBbox.GetExtents().x;
	offset.y += nodeBbox.GetExtents().y;
	offset.z += nodeBbox.GetExtents().z - 1.0F;

	offset += nodeBbox.m_M.getTranslation();

	const Vector3f kRightColumnOffset = Vector3f(ui::eventide::DefaultStyler.text.headingSize * 3.0F, 0.0F, 0.0F);

	// generate layout for the actual thing
	auto nodeProperty = node->sequenceInfo->view->PropertyList()[property_index];
	switch (nodeProperty.renderstyle)
	{
	case m04::editor::PropertyRenderStyle::kBoolean:
		{
			if (Part == PropertyComponent::All)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + Vector3f(m_padding.x, 0, 0),
					offset + Vector3f((nodeBbox.GetExtents().x - m_padding.x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
				);
				return l_bbox;
			}
			else if (Part == PropertyComponent::Key)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + kRightColumnOffset + Vector3f(m_padding.x, 0, 0),
					offset + kRightColumnOffset + Vector3f(m_padding.x + ui::eventide::DefaultStyler.text.buttonSize, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
				);
				return l_bbox;
			}
		}
		break;
	case m04::editor::PropertyRenderStyle::kFloat:
		{
			if (Part == PropertyComponent::All)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + Vector3f(m_padding.x, 0, 0),
					offset + Vector3f((nodeBbox.GetExtents().x - m_padding.x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
				);
				return l_bbox;
			}
			else if (Part == PropertyComponent::Key)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + kRightColumnOffset + Vector3f(m_padding.x, 0, 0),
					offset + Vector3f((nodeBbox.GetExtents().x - m_padding.x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
				);
				return l_bbox;
			}
		}
		break;
	case m04::editor::PropertyRenderStyle::kScriptCharacter:
	case m04::editor::PropertyRenderStyle::kEnumtypeDropdown:
		{
			if (Part == PropertyComponent::All)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + Vector3f(m_padding.x, 0, 0),
					offset + Vector3f((nodeBbox.GetExtents().x - m_padding.x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
				);
				return l_bbox;
			}
			else if (Part == PropertyComponent::Key)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + kRightColumnOffset + Vector3f(m_padding.x, 0, 0),
					offset + Vector3f((nodeBbox.GetExtents().x - m_padding.x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
				);
				return l_bbox;
			}
		}
		break;
	case m04::editor::PropertyRenderStyle::kScriptText:
		{
			const Real l_boxWidth = nodeBbox.GetExtents().x * 2.0F - m_padding.x * 2.0F;
			const char* l_scriptString = node->sequenceInfo->view->GetPropertyAsString(nodeProperty.identifier);
			ParamsForText textParams;
			textParams = {};
			textParams.string = l_scriptString;
			textParams.font_texture = &m_fontTexture;
			textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
			const int l_numberOfLines = std::max(1, (int)(predictText(textParams).x / l_boxWidth) + 1);

			if (Part == PropertyComponent::All)
			{
				core::math::BoundingBox l_bbox(
					Matrix4x4(),
					offset + Vector3f(m_padding.x, 0, 0),
					offset + Vector3f(m_padding.x + l_boxWidth, -ui::eventide::DefaultStyler.text.buttonSize * l_numberOfLines, 4.0F)
				);
				return l_bbox;
			}
		}
		break;
	}

	return core::math::BoundingBox();
}

Real m04::editor::sequence::NodeRenderer::GetBboxOfAllProperties ( void )
{
	Real size = 0.0F;
	auto& nodePropertyList = node->sequenceInfo->view->PropertyList();
	for (uint32_t propertyIndex = 0; propertyIndex < (uint32_t)nodePropertyList.size(); ++propertyIndex)
	{
		size += GetBboxProperty<PropertyComponent::All>(propertyIndex).m_Extent.y * 2.0F;
		size += m_padding.y;
	}
	return size;
}