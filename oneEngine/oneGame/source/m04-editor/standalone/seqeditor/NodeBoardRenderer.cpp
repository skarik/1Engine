#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"
#include "./NodeBoardState.h"
#include "./Constants.h"

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
	m_bbox_flow_input = core::math::BoundingBox(Rotator(), Vector3f(-m_halfsizeOnBoard.x, m_halfsizeOnBoard.y - m_connectionHalfsize.y - m_margins.y, 0), Vector3f(m_connectionHalfsize.x, m_connectionHalfsize.y, 4));
	m_bbox_flow_output = core::math::BoundingBox(Rotator(), Vector3f(m_halfsizeOnBoard.x, m_halfsizeOnBoard.y - m_connectionHalfsize.y - m_margins.y, 0), Vector3f(m_connectionHalfsize.x, m_connectionHalfsize.y, 4));

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
	UpdateNextNode();

	// Set up the properties and extra
	m_propertyState.resize(node->sequenceInfo->view->PropertyList().size());
}

m04::editor::sequence::NodeRenderer::~NodeRenderer ( void )
{
	ReleaseTexture(m_uiElementsTexture);
}

void m04::editor::sequence::NodeRenderer::OnEventMouse ( const EventMouse& mouse_event )
{
	if (mouse_event.type == EventMouse::Type::kDragged)
	{
		if (m_dragging)
		{
			core::math::BoundingBox bbox = GetBBox();
			bbox.m_M.translate(mouse_event.velocity_world);
			bbox.m_MInverse = bbox.m_M.inverse();

			node->position = bbox.GetCenterPoint() - m_halfsizeOnBoard;

			SetBBox( bbox );
			RequestUpdateMesh();
		}
	}
	else
	{
		if (mouse_event.type == EventMouse::Type::kClicked)
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
			
			core::math::BoundingBox l_bbox_property = GetBboxPropertyAll(nodePropertyIndex);
			core::math::BoundingBox l_bbox_property_key = GetBboxPropertyKey(nodePropertyIndex);

			if (l_bbox_property.IsPointInBox(mouse_event.position_world))
			{
				m_propertyState[nodePropertyIndex].m_hovered = true;

				if (mouse_event.type == EventMouse::Type::kClicked)
				{
					if (l_bbox_property_key.IsPointInBox(mouse_event.position_world))
					{
						node->sequenceInfo->view->SetProperty(nodeProperty.identifier, !node->sequenceInfo->view->GetPropertyAsBool(nodeProperty.identifier));
					}
				}
			}
			else
			{
				m_propertyState[nodePropertyIndex].m_hovered = false;
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
	textParams.color = DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.5F);
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
		}
	}
}

void m04::editor::sequence::NodeRenderer::BuildMeshPropertyBoolean ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	/*textParams = {};
	textParams.string = in_property.label.c_str();
	textParams.font_texture = &m_fontTexture;
	textParams.position = in_nodeTopLeft + inout_penPosition + Vector3f(m_margins.x + ui::eventide::DefaultStyler.text.headingSize + 5.0F, -ui::eventide::DefaultStyler.text.headingSize - m_margins.y, 0);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.5F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = DefaultStyler.text.headingColor;
	buildText(textParams);

	quadParams = {};
	quadParams.position = in_nodeTopLeft + inout_penPosition + Vector3f(m_margins.x + ui::eventide::DefaultStyler.text.headingSize * 0.5F, -ui::eventide::DefaultStyler.text.headingSize * 0.5F - m_margins.y, 0.0F);
	quadParams.size = Vector3f(1, 1, 1) * (ui::eventide::DefaultStyler.text.headingSize * 0.5F);
	quadParams.color = Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	quadParams = {};
	quadParams.position = in_nodeTopLeft + inout_penPosition + Vector3f(m_margins.x + ui::eventide::DefaultStyler.text.headingSize * 0.5F, -ui::eventide::DefaultStyler.text.headingSize * 0.5F - m_margins.y, 1.0F);
	quadParams.size = Vector3f(1, 1, 1) * (ui::eventide::DefaultStyler.text.headingSize * 0.5F - 5.0F);
	quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
	quadParams.texture = &m_uiElementsTexture;
	quadParams.color = Color(1, 1, 1, 1);
	buildQuad(quadParams);*/

	const core::math::BoundingBox bbox_all = GetBboxPropertyAll(in_propertyIndex);
	const core::math::BoundingBox bbox_key = GetBboxPropertyKey(in_propertyIndex);

	textParams = {};
	textParams.string = in_property.label.c_str();
	textParams.font_texture = &m_fontTexture;
	textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z) + Vector3f(ui::eventide::DefaultStyler.text.headingSize + m_padding.x, 0, 0);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.5F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
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

void m04::editor::sequence::NodeRenderer::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	core::math::BoundingBox bbox = GetBBox();

	bbox = core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard);
	// TODO: based on properties, update bbox

	SetBBox(bbox);
	RequestUpdateMesh();

	Button::OnGameFrameUpdate(input_frame);

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
		+ Vector3f(0, 0, m_bbox.GetExtents().z + 1)
		+ GetBBoxAbsolute().m_M.getTranslation());
	l_bbox_flow_input.m_MInverse = l_bbox_flow_input.m_M.inverse(); // TODO: optimize this

	return l_bbox_flow_input;
}
core::math::BoundingBox m04::editor::sequence::NodeRenderer::GetBboxFlowOutput ( const uint32_t output_index )
{
	core::math::BoundingBox l_bbox_flow_output = m_bbox_flow_output;
	l_bbox_flow_output.m_M.setTranslation(
		l_bbox_flow_output.m_M.getTranslation() 
		+ Vector3f(0, output_index * m_bbox_flow_output.GetExtents().y * -2.0F, m_bbox.GetExtents().z + 1)
		+ GetBBoxAbsolute().m_M.getTranslation());
	l_bbox_flow_output.m_MInverse = l_bbox_flow_output.m_M.inverse(); // TODO: optimize this

	return l_bbox_flow_output;
}

core::math::BoundingBox m04::editor::sequence::NodeRenderer::GetBboxPropertyAll ( const uint32_t property_index )
{
	// TODO: optimize this
	const core::math::BoundingBox nodeBbox = GetBBoxAbsolute();

	// sum up all the offsets to the current position
	Vector3f offset = Vector3f(0, -ui::eventide::DefaultStyler.text.headingSize - m_margins.y, 0);
	offset.y -= ui::eventide::DefaultStyler.text.headingSize; // Push it down again.
	for (uint32_t checkPropertyIndex = 0; checkPropertyIndex < property_index; ++checkPropertyIndex)
	{
		auto nodeProperty = node->sequenceInfo->view->PropertyList()[property_index];
		switch (nodeProperty.renderstyle)
		{
		case m04::editor::PropertyRenderStyle::kBoolean:
			offset.y -= ui::eventide::DefaultStyler.text.headingSize + m_padding.y;
		}
	}

	// generate layout for the actual thing
	auto nodeProperty = node->sequenceInfo->view->PropertyList()[property_index];
	switch (nodeProperty.renderstyle)
	{
	case m04::editor::PropertyRenderStyle::kBoolean:
		{
			core::math::BoundingBox l_bbox;
			l_bbox.m_Extent = Vector3f(nodeBbox.GetExtents().x - m_padding.x, ui::eventide::DefaultStyler.text.headingSize * 0.5F, 4.0F);
			l_bbox.m_M.setTranslation(nodeBbox.m_M.getTranslation()
				- Vector3f(0, -nodeBbox.GetExtents().y, nodeBbox.GetExtents().z)
				+ Vector3f(0, 0, nodeBbox.GetExtents().z * 2.0F + 1.0F)
				+ offset
				);
			l_bbox.m_MInverse = l_bbox.m_M.inverse();
			return l_bbox;
		}
		break;
	}

	return core::math::BoundingBox();
}
core::math::BoundingBox m04::editor::sequence::NodeRenderer::GetBboxPropertyKey ( const uint32_t property_index )
{
	// TODO: optimize this
		const core::math::BoundingBox nodeBbox = GetBBoxAbsolute();

	// sum up all the offsets to the current position
	Vector3f offset = Vector3f(0, -ui::eventide::DefaultStyler.text.headingSize - m_margins.y, 0);
	offset.y -= ui::eventide::DefaultStyler.text.headingSize; // Push it down again.
	for (uint32_t checkPropertyIndex = 0; checkPropertyIndex < property_index; ++checkPropertyIndex)
	{
		auto nodeProperty = node->sequenceInfo->view->PropertyList()[property_index];
		switch (nodeProperty.renderstyle)
		{
		case m04::editor::PropertyRenderStyle::kBoolean:
			offset.y -= ui::eventide::DefaultStyler.text.headingSize + m_padding.y;
		}
	}

	// generate layout for the actual thing
	auto nodeProperty = node->sequenceInfo->view->PropertyList()[property_index];
	switch (nodeProperty.renderstyle)
	{
	case m04::editor::PropertyRenderStyle::kBoolean:
		{
			core::math::BoundingBox l_bbox;
			l_bbox.m_Extent = Vector3f(ui::eventide::DefaultStyler.text.headingSize * 0.5F, ui::eventide::DefaultStyler.text.headingSize * 0.5F, 4.0F);
			l_bbox.m_M.setTranslation(nodeBbox.m_M.getTranslation()
				- Vector3f(0, -nodeBbox.GetExtents().y, nodeBbox.GetExtents().z)
				+ Vector3f(0, 0, nodeBbox.GetExtents().z * 2.0F + 1.0F)
				+ Vector3f(-nodeBbox.GetExtents().x + m_padding.x + ui::eventide::DefaultStyler.text.headingSize * 0.5F, 0, 0)
				+ offset
				);
			l_bbox.m_MInverse = l_bbox.m_M.inverse();
			return l_bbox;
		}
		break;
	}
	return core::math::BoundingBox();
}