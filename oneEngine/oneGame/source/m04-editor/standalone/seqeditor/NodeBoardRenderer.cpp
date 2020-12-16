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
	, m_connectionHalfsize(Vector3f(10, 10, 10))
{
	m_mouseInteract = MouseInteract::kCapturing;
	m_frameUpdate = FrameUpdate::kPerFrame;

	m_uiElementsTexture = LoadTexture(m04::editor::sequence::gFilenameGUIElementTexture);

	ARCORE_ASSERT(in_node->sequenceInfo.view != NULL); // Must ensure view is valid

	// Set position & bbox
	SetBBox(core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard));

	// Set up the style infos
	m_bbox_flow_input = core::math::BoundingBox(Rotator(), Vector3f(-m_halfsizeOnBoard.x, m_halfsizeOnBoard.y - m_connectionHalfsize.y - m_margins.y, 0), Vector3f(m_connectionHalfsize.x, m_connectionHalfsize.y, 4));
	m_bbox_flow_output = core::math::BoundingBox(Rotator(), Vector3f(m_halfsizeOnBoard.x, m_halfsizeOnBoard.y - m_connectionHalfsize.y - m_margins.y, 0), Vector3f(m_connectionHalfsize.x, m_connectionHalfsize.y, 4));

	// Cache all needed info
	m_guid_text = node->guid.toString().c_str();
	UpdateNextNode();
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
		for (uint32_t flowOutputIndex = 0; flowOutputIndex < node->sequenceInfo.view->Flow().outputCount; ++flowOutputIndex)
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
						hitElementAsNodeRenderer->GetBoardNode()->sequenceInfo.next = &node->sequenceInfo;
						hitElementAsNodeRenderer->UpdateNextNode();
					}
					else if (m_draggingInfo.target == DragState::Target::kFlowOutput)
					{
						node->sequenceInfo.next = &hitElementAsNodeRenderer->GetBoardNode()->sequenceInfo;
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
	textParams.string = "Generic";
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
	for (uint32_t flowOutputIndex = 0; flowOutputIndex < node->sequenceInfo.view->Flow().outputCount; ++flowOutputIndex)
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
		// TODO: when dragging the output to the node
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

	// Draw all the options
	auto& nodeProperties = node->sequenceInfo.view->PropertyList();

}

void m04::editor::sequence::NodeRenderer::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	core::math::BoundingBox bbox = GetBBox();

	bbox = core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard);

	SetBBox(bbox);
	RequestUpdateMesh();

	Button::OnGameFrameUpdate(input_frame);

	m_frameUpdate = FrameUpdate::kPerFrame;
}

void m04::editor::sequence::NodeRenderer::UpdateNextNode ( void )
{
	const NodeBoardState* nbs = m_board;

	// Find the node in the board with the matching pointer
	auto boardNodeIter = (this->node->sequenceInfo.next == NULL)
		? nbs->nodes.end()
		: std::find_if(nbs->nodes.begin(), nbs->nodes.end(), [this](BoardNode* board_node) { return this->node->sequenceInfo.next == &board_node->sequenceInfo; });

	if (boardNodeIter != nbs->nodes.end())
	{
		m_next = (*boardNodeIter)->display;
	}
	else
	{	// Next node does not exist, update both this node and the next one
		m_next = NULL;
		node->sequenceInfo.next = NULL;
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