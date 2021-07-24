#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"
#include "../NodeBoardState.h"
#include "../Constants.h"
#include "../SequenceEditor.h"
#include "core-ext/containers/arStringEnum.h"

#include "./InvalidPropertyRenderer.h"
#include "./ScriptTextPropertyRenderer.h"
#include "./BooleanPropertyRenderer.h"
#include "./FloatPropertyRenderer.h"
#include "./EnumPropertyRenderer.h"

#include "NodeBoardRenderer.h"

const static std::unordered_map<arstring128, m04::editor::sequence::NodeDisplayInfo> g_NodeColors ({
	{"MainTask",			{Color(1.0F, 1.0F, 0.0F)}},
	{"Sidetask",			{Color(1.0F, 1.0F, 0.0F)}},

	{"VN_Lines",			{Color(0.5F, 0.0F, 0.0F)}},
	{"VN_LinesUpdate",		{Color(0.4F, 0.1F, 0.1F)}},

	{"VN_Portrait",			{Color(0.0F, 0.5F, 0.0F)}},
	{"VN_PortraitAction",	{Color(0.1F, 0.5F, 0.1F)}},
	{"VN_Background",		{Color(0.0F, 0.5F, 0.5F)}},

	{"VN_Wait",				{Color(0.0F, 0.0F, 0.0F)}},
});

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

	m_renderResources.m_fontTexture = m_fontTexture;
	m_renderResources.m_uiElementsTexture = LoadTexture(m04::editor::sequence::gFilenameGUIElementTexture);

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
	auto& nodeProperties = node->sequenceInfo->view->PropertyList();
	m_propertyState.resize(nodeProperties.size());
	for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
	{
		IPropertyRenderer* property_renderer = nullptr;
		
		auto& nodeProperty = nodeProperties[nodePropertyIndex];

		IPropertyRenderer::CreationParameters l_propCreateParams = {this, &nodeProperty, &m_propertyState[nodePropertyIndex]};

		switch (nodeProperty.renderstyle)
		{
		case m04::editor::PropertyRenderStyle::kBoolean:
			property_renderer = new m04::editor::sequence::BooleanPropertyRenderer(l_propCreateParams);
			break;

		case m04::editor::PropertyRenderStyle::kFloat:
			property_renderer = new m04::editor::sequence::FloatPropertyRenderer(l_propCreateParams);
			break;

		case m04::editor::PropertyRenderStyle::kScriptText:
			property_renderer = new m04::editor::sequence::ScriptTextPropertyRenderer(l_propCreateParams);
			break;

		case m04::editor::PropertyRenderStyle::kEnumtypeDropdown:
		case m04::editor::PropertyRenderStyle::kScriptCharacter:
			property_renderer = new m04::editor::sequence::EnumPropertyRenderer(l_propCreateParams);
			break;

		default:
			property_renderer = new m04::editor::sequence::InvalidPropertyRenderer(l_propCreateParams);
			break;
		}

		m_propertyRenderers.push_back(property_renderer);
	}

	UpdatePropertyLayout();
}

m04::editor::sequence::NodeRenderer::~NodeRenderer ( void )
{
	ReleaseTexture(m_renderResources.m_uiElementsTexture);

	for (IPropertyRenderer* renderer : m_propertyRenderers)
	{
		delete renderer;
	}
	m_propertyRenderers.clear();
}

void m04::editor::sequence::NodeRenderer::UpdateCachedVisualInfo ( void )
{
	using namespace ui::eventide;

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

	// Grab tint color from the node info
	auto& tintColor = g_NodeColors.find(node->sequenceInfo->view->classname);
	if (tintColor != g_NodeColors.end())
	{
		m_display_tint = tintColor->second.color;
	}
	else
	{
		m_display_tint = DefaultStyler.box.defaultColor;
	}
}

void m04::editor::sequence::NodeRenderer::UpdatePropertyLayout()
{
	const core::math::BoundingBox nodeBbox = GetBBoxAbsolute();
	const Real kRightColumnOffset = ui::eventide::DefaultStyler.text.headingSize * 3.0F;

	// sum up all the offsets to the current position
	Vector3f offset;
	offset.y = -ui::eventide::DefaultStyler.text.headingSize - m_margins.y * 2.0F; // Start below the title.

	// Apply the offset so we're working against the upper-left corner
	offset.x -= nodeBbox.GetExtents().x;
	offset.y += nodeBbox.GetExtents().y;
	offset.z += nodeBbox.GetExtents().z - 1.0F;
	// Apply offset for the world-space position
	offset += nodeBbox.m_M.getTranslation();

	for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < m_propertyRenderers.size(); ++nodePropertyIndex)
	{
		m_propertyRenderers[nodePropertyIndex]->UpdateLayout(offset, kRightColumnOffset, nodeBbox);

		offset.y -= m_propertyRenderers[nodePropertyIndex]->GetCachedBboxHeight();
	}
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
			UpdatePropertyLayout();
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

		// TODO: a lot of the following nodeProperty.renderstyle if-statement can be moved to a separate class/abstracted away, instead of being a massive mess here.
		auto& nodeProperties = node->sequenceInfo->view->PropertyList();
		for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
		{
			auto& nodeProperty = nodeProperties[nodePropertyIndex];
			
			core::math::BoundingBox l_bbox_property = GetBboxProperty<PropertyComponent::All>(nodePropertyIndex);
			core::math::BoundingBox l_bbox_property_key = GetBboxProperty<PropertyComponent::Key>(nodePropertyIndex);

			if (l_bbox_property.IsPointInBox(mouse_event.position_world))
			{
				m_propertyState[nodePropertyIndex].m_hovered = true;

				if (mouse_event.type == EventMouse::Type::kClicked)
				{
					m_propertyRenderers[nodePropertyIndex]->OnClicked(mouse_event);
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
	bool bMouseInThisElement = mouse_event.element == this;

	if (bMouseInThisElement)
	{
		m_selected = true;
		m_mouseInteract = MouseInteract::kCapturingCatchAll;

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
	else
	{
		m_selected = false;
		m_mouseInteract = MouseInteract::kCapturing;
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
						if (hitElementAsNodeRenderer != this)
						{
							hitElementAsNodeRenderer->GetBoardNode()->sequenceInfo->next = node->sequenceInfo;
							hitElementAsNodeRenderer->UpdateNextNode();
						}
					}
					else if (m_draggingInfo.target == DragState::Target::kFlowOutput)
					{
						if (hitElementAsNodeRenderer != this)
						{
							node->sequenceInfo->next = hitElementAsNodeRenderer->GetBoardNode()->sequenceInfo;
							m_next = hitElementAsNodeRenderer;
						}
					}
				}
			}
		}

		m_draggingInfo.active = false;
		m_mouseInteract = MouseInteract::kCapturing;
		m_selected = false;
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
		.Lerp(m_display_tint, 0.5F);
		//.Lerp(DefaultStyler.box.activeColor, Styler::PulseFade(m_activateGlowPulse));
	//cubeParams.wireframe = true; // TODO
	buildCube(cubeParams);

	if (m_selected)
	{
		// Draw an outline of the box.
		core::math::Cubic baseCube = cubeParams.box;

		const Real outlineWidth = 2.0F;

		cubeParams.color = Color(1.0F, 1.0F, 1.0F, 1.0F);
		cubeParams.box = core::math::Cubic(
			baseCube.position + Vector3f(-outlineWidth, -outlineWidth, 0.0F),
			Vector3f(outlineWidth, baseCube.size.y + outlineWidth * 2.0F, baseCube.size.z));
		buildCube(cubeParams);

		cubeParams.box = core::math::Cubic(
			baseCube.position + Vector3f(baseCube.size.x, -outlineWidth, 0.0F),
			Vector3f(outlineWidth, baseCube.size.y + outlineWidth * 2.0F, baseCube.size.z));
		buildCube(cubeParams);

		cubeParams.box = core::math::Cubic(
			baseCube.position + Vector3f(0.0F, -outlineWidth, 0.0F),
			Vector3f(baseCube.size.x, outlineWidth, baseCube.size.z));
		buildCube(cubeParams);

		cubeParams.box = core::math::Cubic(
			baseCube.position + Vector3f(0.0F, baseCube.size.y, 0.0F),
			Vector3f(baseCube.size.x, outlineWidth, baseCube.size.z));
		buildCube(cubeParams);

	}

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
		quadParams.position = l_bbox_flow_input.GetCenterPoint() + Vector3f(l_bbox_flow_input.GetExtents().x * 0.5F, 0, 0);
		quadParams.size = Vector2f(l_bbox_flow_input.GetExtents().x * 0.5F, l_bbox_flow_input.GetExtents().y);
		quadParams.uvs = Rect((128.0F + 64.0F) / 1024, 0.0F, 64.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_renderResources.m_uiElementsTexture;
		quadParams.color = DefaultStyler.box.defaultColor;
		buildQuad(quadParams);
	}

	// Flow output
	for (uint32_t flowOutputIndex = 0; flowOutputIndex < node->sequenceInfo->view->Flow().outputCount; ++flowOutputIndex)
	{
		core::math::BoundingBox l_bbox_flow_output = GetBboxFlowOutput(flowOutputIndex);

		quadParams = {};
		quadParams.position = l_bbox_flow_output.GetCenterPoint() - Vector3f(l_bbox_flow_output.GetExtents().x * 0.5F, 0, 0);
		quadParams.size = Vector2f(l_bbox_flow_output.GetExtents().x * 0.5F, l_bbox_flow_output.GetExtents().y);
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 64.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_renderResources.m_uiElementsTexture;
		quadParams.color = DefaultStyler.box.defaultColor;
		buildQuad(quadParams);
	}

	// Draw drag nonsense
	if (m_draggingInfo.active)
	{
		/*cubeParams = {};
		cubeParams.box = core::math::Cubic::ConstructCenterExtents(m_ui->GetMousePosition(), Vector3f(5, 5, 5));
		cubeParams.color = Color(1, 0, 0, 1);
		cubeParams.texture = NULL;
		buildCube(cubeParams);*/

		// Build the curve
		pathParams = {};
		Vector3f pointArray[21];
		pointArray[0] = (m_draggingInfo.target == DragState::Target::kFlowInput)
			? GetBboxFlowInput().GetCenterPoint()
			: GetBboxFlowOutput(0).GetCenterPoint();
		pointArray[20] = m_board->GetEditor()->GetMousePosition3D(); // Lead curve to the mouse position
		const Vector3f pointDelta = pointArray[20] - pointArray[0];
		// Build a bezier curve between the points
		for (int i = 1; i < 20; ++i)
		{
			Vector3f& pointA = pointArray[0];
			Vector3f& pointB = pointArray[20];
			const Vector3f anchorA = pointA + Vector3f(pointDelta.x, 0, 0);
			const Vector3f anchorB = pointB - Vector3f(pointDelta.x, 0, 0);
			Real interpolator = i / 20.0F;

			pointArray[i] = 
				Vector3f::lerp(
					Vector3f::lerp(
						Vector3f::lerp(pointA, anchorA, interpolator),
						Vector3f::lerp(anchorA, anchorB, interpolator), interpolator),
					Vector3f::lerp(
						Vector3f::lerp(anchorA, anchorB, interpolator),
						Vector3f::lerp(anchorB, pointB, interpolator), interpolator), interpolator);
		}
		pathParams.points = pointArray;
		pathParams.pointCount = 21;
		buildPath(pathParams);
	}

	// Draw the flow output line
	if (m_next == NULL)
	{
		;	
	}
	else
	{
		NodeRenderer* next_noderenderer = (NodeRenderer*)m_next;

		/*cubeParams = {};
		cubeParams.box = core::math::Cubic::ConstructCenterExtents((GetBboxFlowOutput(0).GetCenterPoint() + next_noderenderer->GetBboxFlowInput().GetCenterPoint()) * 0.5F, Vector3f(5, 5, 5));
		cubeParams.color = Color(1, 0, 0, 1);
		cubeParams.texture = NULL;
		buildCube(cubeParams);*/

		core::math::BoundingBox l_bbox_flow_output = GetBboxFlowOutput(0);
		core::math::BoundingBox l_bbox_flow_input = next_noderenderer->GetBboxFlowInput();

		// Build the click-able node
		quadParams = {};
		quadParams.position = (l_bbox_flow_output.GetCenterPoint() + l_bbox_flow_input.GetCenterPoint()) * 0.5F + Vector3f(0, 0, 2);
		quadParams.size = Vector2f(l_bbox_flow_output.GetExtents().x, l_bbox_flow_output.GetExtents().y) * 0.5F;
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_renderResources.m_uiElementsTexture;
		quadParams.color = Color(1.0F, 1.0F, 1.0F, 1.0F);
		buildQuad(quadParams);

		// Build the curve
		pathParams = {};
		/*Vector3f pointArray[] = {
			Vector3f(GetBboxFlowOutput(0).GetCenterPoint()),
			Vector3f((GetBboxFlowOutput(0).GetCenterPoint() + next_noderenderer->GetBboxFlowInput().GetCenterPoint()) * 0.5F),
			Vector3f(next_noderenderer->GetBboxFlowInput().GetCenterPoint()),
		};*/
		Vector3f pointArray[21];
		pointArray[0] = GetBboxFlowOutput(0).GetCenterPoint();
		pointArray[20] = next_noderenderer->GetBboxFlowInput().GetCenterPoint();
		const Vector3f pointDelta = pointArray[20] - pointArray[0];
		// Build a bezier curve between the points
		for (int i = 1; i < 20; ++i)
		{
			Vector3f& pointA = pointArray[0];
			Vector3f& pointB = pointArray[20];
			const Vector3f anchorA = pointA + Vector3f(pointDelta.x, 0, 0);
			const Vector3f anchorB = pointB - Vector3f(pointDelta.x, 0, 0);
			Real interpolator = i / 20.0F;

			pointArray[i] = 
				Vector3f::lerp(
					Vector3f::lerp(
						Vector3f::lerp(pointA, anchorA, interpolator),
						Vector3f::lerp(anchorA, anchorB, interpolator), interpolator),
					Vector3f::lerp(
						Vector3f::lerp(anchorA, anchorB, interpolator),
						Vector3f::lerp(anchorB, pointB, interpolator), interpolator), interpolator);
		}
		pathParams.points = pointArray;
		pathParams.pointCount = 21;
		buildPath(pathParams);
	}
	// TODO: when dragging the output to the node

	// Draw all the options
	auto& nodeProperties = node->sequenceInfo->view->PropertyList();
	for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
	{
		auto& nodeProperty = nodeProperties[nodePropertyIndex];
		m_propertyRenderers[nodePropertyIndex]->BuildMesh();
	}
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
	// Update the property positions (TODO: call this only when bbox changes)
	UpdatePropertyLayout();
	// Request the mesh updates next frame
	RequestUpdateMesh();

	// Run the button updates
	Button::OnGameFrameUpdate(input_frame);

	if (!m_board->GetEditor()->GetDuskUI()->IsInDialogue())
	{
		// TODO: more organized
		// Update per-frame edits
		auto& nodeProperties = node->sequenceInfo->view->PropertyList();
		for (uint32_t nodePropertyIndex = 0; nodePropertyIndex < nodeProperties.size(); ++nodePropertyIndex)
		{
			auto& nodeProperty = nodeProperties[nodePropertyIndex];
			m_propertyRenderers[nodePropertyIndex]->OnGameFrameUpdate(input_frame);
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
	if (Part == PropertyComponent::All)
	{
		return m_propertyRenderers[property_index]->GetCachedBboxAll();
	}
	else if (Part == PropertyComponent::Key)
	{
		return m_propertyRenderers[property_index]->GetCachedBboxKey();
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