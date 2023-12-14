#include "./ArrayPropertyRenderer.h"
#include "./NodeBoardRenderer.h"
#include "../SequenceNodeExternalDefinition.h"

void m04::editor::sequence::ArrayPropertyRenderer::OnClicked ( const ui::eventide::Element::EventMouse& mouse_event )
{
	typedef ui::eventide::Element::EventMouse EventMouse;

	ARCORE_ASSERT(mouse_event.type == EventMouse::Type::kClicked);

	if (mouse_event.button == core::kMBLeft)
	{
		//m_propertyState->m_editing = true;
		core::math::BoundingBox addRect(Rotator(), m_bboxKey.GetCenterPoint(), Vector3f(1, 1, 1) * ui::eventide::DefaultStyler.text.buttonSize);

		if (addRect.IsPointInBox(mouse_event.position_world))
		{
			// TODO: Signal to UI to wait for rendering to finish.
			//GetNode()->view->SetProperty(m_property->identifier, !GetNode()->view->GetPropertyAsBool(m_property->identifier));
			auto arrayValue = m_targetData->GetAdd<osf::ArrayValue>(m_property->identifier);
			arrayValue->values.push_back(new osf::ObjectValue());
			return;
		}
	}

	// Forward the event to the objects
	auto arrayValue = m_targetData->GetAdd<osf::ArrayValue>(m_property->identifier);
	for (size_t elementIndex = 0; elementIndex < arrayValue->values.size(); ++elementIndex)
	{
		for (size_t subpropertyIndex = 0; subpropertyIndex < m_property->definition->arraySubproperties->size(); ++subpropertyIndex)
		{
			size_t subpropertyRendererIndex = elementIndex * m_property->definition->arraySubproperties->size() + subpropertyIndex;
			if (m_subproperties[subpropertyRendererIndex] != nullptr)
			{
				// Force update the subproperty states
				if (m_subproperties[subpropertyRendererIndex]->GetCachedBboxAll().IsPointInBox(mouse_event.position_world))
				{
					m_subpropertyState[subpropertyRendererIndex].m_hovered = true;
					m_subproperties[subpropertyRendererIndex]->OnClicked(mouse_event);
				}
				else
				{
					m_subpropertyState[subpropertyRendererIndex].m_hovered = false;
					m_subpropertyState[subpropertyRendererIndex].m_editing = false;
					m_subproperties[subpropertyRendererIndex]->OnClickedOutside(mouse_event);
				}
			}
		}
	}
}

void m04::editor::sequence::ArrayPropertyRenderer::OnClickedOutside ( const ui::eventide::Element::EventMouse& mouse_event )
{
	// Forward the event to the objects
	auto arrayValue = m_targetData->GetAdd<osf::ArrayValue>(m_property->identifier);
	for (size_t elementIndex = 0; elementIndex < arrayValue->values.size(); ++elementIndex)
	{
		for (size_t subpropertyIndex = 0; subpropertyIndex < m_property->definition->arraySubproperties->size(); ++subpropertyIndex)
		{
			size_t subpropertyRendererIndex = elementIndex * m_property->definition->arraySubproperties->size() + subpropertyIndex;
			if (m_subproperties[subpropertyRendererIndex] != nullptr)
			{
				// Force update the subproperty states
				if (!m_subproperties[subpropertyRendererIndex]->GetCachedBboxAll().IsPointInBox(mouse_event.position_world))
				{
					m_subpropertyState[subpropertyRendererIndex].m_hovered = false;
					m_subpropertyState[subpropertyRendererIndex].m_editing = false;
					m_subproperties[subpropertyRendererIndex]->OnClickedOutside(mouse_event);
				}
			}
		}
	}
}

void m04::editor::sequence::ArrayPropertyRenderer::BuildMesh ( void )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	std::string tempString;

	const Vector3f kZStep = Vector3f(0, 0, 0.1F);

	textParams.setDefaults();
	textParams.string = m_property->label.c_str();
	textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
	textParams.position = m_bboxAll.GetCenterPoint() - Vector3f(m_bboxAll.GetExtents().x, m_bboxAll.GetExtents().y, 0) + kZStep;
	textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);

	// Add the +/- buttons at the top for the array, and display number of values
	//GetNode()->view->GetPropertyArray(
	auto arrayValue = m_targetData->GetAdd<osf::ArrayValue>(m_property->identifier);
	//auto arrayKeyValue = GetNode()->data.GetKeyValueAdd(m_property->identifier);
	//auto arrayValue = 
	// display size of the array

	//tempString = "Array Size<" + std::to_string(arrayValue->values.size()) + ">";

	// Display the size of the array.
	tempString = "Array, " + std::to_string(arrayValue->values.size()) + " elements";
	textParams.setDefaults()
		.setString(tempString.c_str())
		.setFontTexture(&m_nodeRenderer->GetRenderResources().m_fontTexture)
		.setTransform(
			m_bboxKey.GetCenterPoint() - Vector3f(m_bboxKey.GetExtents().x, m_bboxKey.GetExtents().y, 0) + kZStep,
			m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator())
		.setSize(math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize))
		.setAlignment(AlignHorizontal::kLeft)
		.setColor(m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F));
	buildText(textParams);


	for (auto subValue : arrayValue->values)
	{
		// Draw each object group, pull the elements from the property definition:
		for (auto subProperty : *m_property->definition->arraySubproperties)
		{

		}
	}

	// Display the "add element" button
	quadParams = ParamsForQuad();
	quadParams.position = m_bboxKey.GetCenterPoint() + Vector3f(m_bboxKey.GetExtents().x * 0.5f, 0, 0);
	quadParams.size = Vector3f(1, 1, 1) * (ui::eventide::DefaultStyler.text.buttonSize);
	quadParams.color = Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	for (size_t elementIndex = 0; elementIndex < arrayValue->values.size(); ++elementIndex)
	{
		for (size_t subpropertyIndex = 0; subpropertyIndex < m_property->definition->arraySubproperties->size(); ++subpropertyIndex)
		{
			size_t subpropertyRendererIndex = elementIndex * m_property->definition->arraySubproperties->size() + subpropertyIndex;
			if (subpropertyRendererIndex < m_subproperties.size() && m_subproperties[subpropertyRendererIndex] != nullptr)
			{
				m_subproperties[subpropertyRendererIndex]->BuildMesh();
			}
		}
	}

	/*quadParams = ParamsForQuad();
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
	}*/
}

void m04::editor::sequence::ArrayPropertyRenderer::OnGameFrameUpdate ( const ui::eventide::Element::GameFrameUpdateInput& input_frame )
{
	/*if (m_propertyState->m_editing)
	{
		// Parse input and modify the underlying property
		std::string l_currentValue = GetNode()->view->GetPropertyAsString(m_property->identifier);

		// Run the parse loop (see dusk/TextField.cpp)
		const auto& inputString = core::Input::FrameInputString( input_frame.input_index );
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
	}*/

	// Set up the needed renderers for all of the subproperties
	auto arrayValue = GetNode()->data.GetAdd<osf::ArrayValue>(m_property->identifier);

	size_t total_subproperties = arrayValue->values.size() * m_property->definition->arraySubproperties->size();
	if (m_subproperties.size() != total_subproperties)
	{
		m_subproperties.resize(0, nullptr); // TODO: Leaks
		m_subproperties.resize(total_subproperties, nullptr);
		m_subpropertyState.resize(total_subproperties);
		m_subpropertyProperties.resize(total_subproperties);
	}

	for (size_t elementIndex = 0; elementIndex < arrayValue->values.size(); ++elementIndex)
	{
		for (size_t subpropertyIndex = 0; subpropertyIndex < m_property->definition->arraySubproperties->size(); ++subpropertyIndex)
		{
			size_t subpropertyRendererIndex = elementIndex * m_property->definition->arraySubproperties->size() + subpropertyIndex;
			if (m_subproperties[subpropertyRendererIndex] == nullptr)
			{
				m_subpropertyProperties[subpropertyRendererIndex].definition = &m_property->definition->arraySubproperties->at(subpropertyIndex);
				m_subpropertyProperties[subpropertyRendererIndex].label = m_property->definition->arraySubproperties->at(subpropertyIndex).displayName;
				m_subpropertyProperties[subpropertyRendererIndex].identifier = m_property->definition->arraySubproperties->at(subpropertyIndex).name;
				m_subpropertyProperties[subpropertyRendererIndex].renderstyle = m_property->definition->arraySubproperties->at(subpropertyIndex).type;

				PropertyRendererCreateParams params;
				params.node_renderer = this->m_nodeRenderer;
				params.property = &m_subpropertyProperties[subpropertyRendererIndex];  //m_property->definition->arraySubproperties->at(subpropertyIndex);
				params.property_state = &m_subpropertyState[subpropertyRendererIndex];
				params.target_data = m_targetData->GetAdd<osf::ArrayValue>(m_property->identifier)->values[elementIndex]->As<osf::ObjectValue>();

				m_subproperties[subpropertyRendererIndex] = m04::editor::sequence::CreatePropertyRenderer(m_property->definition->arraySubproperties->at(subpropertyIndex).type, params);
			}

			if (m_subproperties[subpropertyRendererIndex] != nullptr)
			{
				m_subproperties[subpropertyRendererIndex]->OnGameFrameUpdate(input_frame);
			}
		}
	}
}

void m04::editor::sequence::ArrayPropertyRenderer::UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox )
{
	auto arrayValue = m_targetData->GetAdd<osf::ArrayValue>(m_property->identifier);
	Real lc_width = left_column_width;

	// Start with an estimated bbox with the correct width:
	int line_height = 1 + (int)arrayValue->values.size() * (int)m_property->definition->arraySubproperties->size();
	m_bboxHeight = (ui::eventide::DefaultStyler.text.buttonSize + m_nodeRenderer->GetPadding().y) * line_height;
	m_bboxAll = core::math::BoundingBox(
		Matrix4x4(),
		upper_left_corner + Vector3f(m_nodeRenderer->GetPadding().x, 0, 0),
		upper_left_corner + Vector3f(node_bbox.GetExtents().x * 2.0F - m_nodeRenderer->GetPadding().x - lc_width, -m_bboxHeight, 4.0F)
	);

	// Collect the height of all the subelements & place them in [roughly] correct positions:
	float total_subelement_height = ui::eventide::DefaultStyler.text.buttonSize + m_nodeRenderer->GetPadding().y;
	Vector3f ul_corner = upper_left_corner - Vector3f(-left_column_width, (ui::eventide::DefaultStyler.text.buttonSize + m_nodeRenderer->GetPadding().y), 0);

	for (size_t elementIndex = 0; elementIndex < arrayValue->values.size(); ++elementIndex)
	{
		for (size_t subpropertyIndex = 0; subpropertyIndex < m_property->definition->arraySubproperties->size(); ++subpropertyIndex)
		{
			size_t subpropertyRendererIndex = elementIndex * m_property->definition->arraySubproperties->size() + subpropertyIndex;
			if (subpropertyRendererIndex < m_subproperties.size() && m_subproperties[subpropertyRendererIndex] != nullptr)
			{
				m_subproperties[subpropertyRendererIndex]->UpdateLayout(
					ul_corner,
					lc_width,
					m_bboxAll
				);

				float elementHeight = m_subproperties[subpropertyRendererIndex]->GetCachedBboxAll().GetExtents().y * 2.0F + m_nodeRenderer->GetPadding().y;

				ul_corner.y -= elementHeight;
				total_subelement_height += elementHeight;
			}
		}
	}

	// Resize the bboxes
	{
		// Total height:
		m_bboxHeight = total_subelement_height;

		// Total container:
		m_bboxAll = core::math::BoundingBox(
			Matrix4x4(),
			upper_left_corner
				+ Vector3f(m_nodeRenderer->GetPadding().x, 0, 0),
			upper_left_corner
				+ Vector3f(node_bbox.GetExtents().x * 2.0F - m_nodeRenderer->GetPadding().x - lc_width, -m_bboxHeight, 4.0F)
		);

		// Interact add/remove button at the top:
		m_bboxKey = core::math::BoundingBox(
			Matrix4x4(),
			upper_left_corner
				+ Vector3f(left_column_width, 0, 0) + Vector3f(m_nodeRenderer->GetPadding().x, 0, 0),
			upper_left_corner
				+ Vector3f(node_bbox.GetExtents().x * 2.0F - m_nodeRenderer->GetPadding().x - lc_width, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
		);
	}
}