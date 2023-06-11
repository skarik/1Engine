#include "./EnumPropertyRenderer.h"
#include "./NodeBoardRenderer.h"

#include "../NodeBoardState.h"
#include "../SequenceEditor.h"

#include "core-ext/containers/arStringEnum.h"

m04::editor::sequence::EnumPropertyRenderer::EnumPropertyRenderer ( const PropertyRendererCreateParams& params )
	: IPropertyRenderer(params)
{
	// Find the matching enum definition
	const auto& editorEnums = m_nodeRenderer->GetNodeBoardState()->GetEditor()->GetEnums();
	arstring128 enumDefinitionName;

	std::string identifierLower = m_property->identifier;
	core::utils::string::ToLower(identifierLower);

	if (editorEnums.find(identifierLower.c_str()) != editorEnums.end())
	{
		enumDefinitionName = identifierLower.c_str();
	}
	else
	{
		// Take display name and remove the spaces
		std::string moddedDisplayName = m_property->label;
		moddedDisplayName.erase(std::remove(moddedDisplayName.begin(), moddedDisplayName.end(), ' '));
		core::utils::string::ToLower(moddedDisplayName);

		if (editorEnums.find(moddedDisplayName.c_str()) != editorEnums.end())
		{
			enumDefinitionName = moddedDisplayName.c_str();
		}
	}

	//ARCORE_ASSERT(enumDefinitionName.length() > 0); // TODO
	auto enumEntry = editorEnums.find(enumDefinitionName);
	if (enumEntry != editorEnums.end())
	{
		m_enumDefinition = enumEntry->second;
	}
}

void m04::editor::sequence::EnumPropertyRenderer::OnClicked ( const ui::eventide::Element::EventMouse& mouse_event )
{
	typedef ui::eventide::Element::EventMouse EventMouse;

	ARCORE_ASSERT(mouse_event.type == EventMouse::Type::kClicked);

	if (mouse_event.button == core::kMBLeft)
	{
		if (m_enumDefinition != NULL)
		{
			// find matching enum name
			int32_t currentEnumIndex = -1;
			const char* str = GetNode()->view->GetPropertyAsString(m_property->identifier);
			if (str == NULL || strlen(str) == 0)
			{
				currentEnumIndex = 0;
			}
			else
			{
				auto enumValue = m_enumDefinition->enumDefinition->CreateValue(str);
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
			auto enumValue = m_enumDefinition->enumDefinition->CreateValueFromIndex(currentEnumIndex);
			if (enumValue.IsValid())
			{
				GetNode()->view->SetProperty(m_property->identifier, enumValue.GetName());
			}
			else
			{
				GetNode()->view->SetProperty(m_property->identifier, m_enumDefinition->enumDefinition->CreateValueFromIndex(0).GetName());
			}
		}
	}
}

void m04::editor::sequence::EnumPropertyRenderer::BuildMesh ( void )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	textParams = ParamsForText();
	textParams.string = m_property->label.c_str();
	textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
	textParams.position = m_bboxAll.GetCenterPoint() - Vector3f(m_bboxAll.GetExtents().x, m_bboxAll.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
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

	// Now, render the current setting
	if (m_enumDefinition != NULL)
	{
		bool makeReadable = false;

		// find matching enum name
		const char* str = GetNode()->view->GetPropertyAsString(m_property->identifier);
		if (str == NULL || strlen(str) == 0)
		{
			str = m_enumDefinition->enumDefinition->GetEnumName(0); //todo
			makeReadable = true;
		}
		else
		{
			auto enumValue = m_enumDefinition->enumDefinition->CreateValue(str);
			if (enumValue.IsValid())
			{
				auto enumDisplayName = m_enumDefinition->displayNames[enumValue.GetEnumIndex()];
				if (enumDisplayName.length() <= 0)
				{
					str = enumValue.GetName();
					makeReadable = true;
				}
				else
				{
					str = enumDisplayName;
				}
			}
			else
			{
				str = m_enumDefinition->enumDefinition->GetEnumName(0); //todo
				makeReadable = true;
			}
		}

		arstring256 camelCasedValue = makeReadable ? core::utils::string::CamelCaseToReadable(str, strlen(str)) : str;
		camelCasedValue[0] = ::toupper(camelCasedValue[0]);

		textParams = ParamsForText();
		textParams.string = camelCasedValue.c_str();
		textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
		textParams.position = m_bboxKey.GetCenterPoint() - Vector3f(m_bboxKey.GetExtents().x, m_bboxKey.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
		textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
		buildText(textParams);
	}
}

void m04::editor::sequence::EnumPropertyRenderer::UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox )
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