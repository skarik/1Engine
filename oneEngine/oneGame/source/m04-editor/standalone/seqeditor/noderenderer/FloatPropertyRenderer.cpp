#include "./FloatPropertyRenderer.h"
#include "./NodeBoardRenderer.h"

void m04::editor::sequence::FloatPropertyRenderer::OnClicked ( const ui::eventide::Element::EventMouse& mouse_event )
{
	typedef ui::eventide::Element::EventMouse EventMouse;

	ARCORE_ASSERT(mouse_event.type == EventMouse::Type::kClicked);

	if (mouse_event.button == core::kMBLeft)
	{
		m_propertyState->m_editing = true;
	}
}

void m04::editor::sequence::FloatPropertyRenderer::BuildMesh ( Vector3f& inout_penPosition )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	textParams = ParamsForText();
	textParams.string = m_property->label.c_str();
	textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
	//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z);
	textParams.position = m_bboxAll.GetCenterPoint() - Vector3f(m_bboxAll.GetExtents().x, m_bboxAll.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);

	quadParams = ParamsForQuad();
	quadParams.position = m_bboxKey.GetCenterPoint();
	quadParams.size = m_bboxKey.GetExtents();
	quadParams.color = m_propertyState->m_hovered ? Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F) : DefaultStyler.box.defaultColor;
	buildQuad(quadParams);

	const char* str = NULL;
	std::string str_tempBuffer;
	if (m_propertyState->m_editing)
	{
		str = GetNode()->view->GetPropertyAsString(m_property->identifier);
	}
	else
	{
		const float currentValue = GetNode()->view->GetPropertyAsFloat(m_property->identifier);
		str_tempBuffer = std::to_string(currentValue);
		str_tempBuffer.erase(str_tempBuffer.find_last_not_of('0') + 1, std::string::npos);
		str_tempBuffer.erase(str_tempBuffer.find_last_not_of('.') + 1, std::string::npos); // See the node conversion GetPropertyAsFloat/GetPropertyAsString
		str = str_tempBuffer.c_str();
	}

	if (str)
	{
		std::string l_drawString = str;
		if (m_propertyState->m_editing)
		{
			if (fmod(Time::currentTime, 1.0F) < 0.5F)
			{
				l_drawString += '|';
			}
		}

		textParams = ParamsForText();
		textParams.string = l_drawString.c_str();
		textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
		textParams.position = m_bboxKey.GetCenterPoint() - Vector3f(m_bboxKey.GetExtents().x, m_bboxKey.GetExtents().y, 0) + Vector3f(0, 0, 0.2F);
		textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = Color(1, 1, 1, 1);
		buildText(textParams);
	}

	// Push down pen
	inout_penPosition.y -= ui::eventide::DefaultStyler.text.buttonSize + 5;
}

void m04::editor::sequence::FloatPropertyRenderer::OnGameFrameUpdate ( const ui::eventide::Element::GameFrameUpdateInput& input_frame )
{
	if (m_propertyState->m_editing)
	{
		// Parse input and modify the underlying property
		std::string l_currentValue = GetNode()->view->GetPropertyAsString(m_property->identifier);

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
		GetNode()->view->SetProperty(m_property->identifier, l_currentValue.c_str());
	}
	else
	{
		// If the field is invalid, force the value of zero
		if (GetNode()->view->node->data[m_property->identifier]->GetType() == osf::ValueType::kString)
		{
			std::string l_currentValue = GetNode()->view->GetPropertyAsString(m_property->identifier);
			try
			{
				std::stof(l_currentValue); // Try the conversion. If it throws, the input is invalid.
			}
			catch (std::invalid_argument&)
			{
				GetNode()->view->SetProperty(m_property->identifier, "0");
			}
		}
		// Forces the value to convert back to a floating point value.
		GetNode()->view->GetPropertyAsFloat(m_property->identifier);
	}
}

void m04::editor::sequence::FloatPropertyRenderer::UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox )
{
	m_bboxHeight = ui::eventide::DefaultStyler.text.buttonSize + m_nodeRenderer->GetPadding().y;

	m_bboxAll = core::math::BoundingBox(
		Matrix4x4(),
		upper_left_corner + Vector3f(m_nodeRenderer->GetPadding().x, 0, 0),
		upper_left_corner + Vector3f((node_bbox.GetExtents().x - m_nodeRenderer->GetPadding().x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
	);

	m_bboxKey = core::math::BoundingBox(
		Matrix4x4(),
		upper_left_corner + Vector3f(left_column_width, 0, 0) + Vector3f(m_nodeRenderer->GetPadding().x, 0, 0),
		upper_left_corner + Vector3f((node_bbox.GetExtents().x - m_nodeRenderer->GetPadding().x) * 2.0F, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
	);
}