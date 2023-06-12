#include "./BooleanPropertyRenderer.h"
#include "./NodeBoardRenderer.h"

void m04::editor::sequence::BooleanPropertyRenderer::OnClicked ( const ui::eventide::Element::EventMouse& mouse_event )
{
	typedef ui::eventide::Element::EventMouse EventMouse;

	ARCORE_ASSERT(mouse_event.type == EventMouse::Type::kClicked);

	if (mouse_event.button == core::kMBLeft)
	{
		if (m_bboxKey.IsPointInBox(mouse_event.position_world))
		{
			// We need this to work within an array - so, instead of a "property view" that works on any OSF object so
			//	using m04::editor::sequence::properties;
			//	SetProperty(GetNode()->data, m_property->identifier, !GetProperty<bool>(GetNode()->data));
			//	SetProperty(m_targetData, m_property->identifier, !GetProperty<bool>(m_targetData));
			//GetNode()->view->SetProperty(m_property->identifier, !GetNode()->view->GetPropertyAsBool(m_property->identifier));
			
			using namespace m04::editor::sequence::properties;
			SetProperty(m_targetData, m_property->identifier, !GetProperty<bool>(m_targetData, m_property->identifier));
		}
	}
}

void m04::editor::sequence::BooleanPropertyRenderer::BuildMesh ( void )
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

	quadParams = ParamsForQuad();
	quadParams.position = m_bboxKey.GetCenterPoint();
	quadParams.size = Vector3f(1, 1, 1) * (m_bboxKey.GetExtents().y);
	quadParams.color = Color(1, 1, 1, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	//if (GetNode()->view->GetPropertyAsBool(m_property->identifier))
	if (properties::GetProperty<bool>(m_targetData, m_property->identifier))
	{
		quadParams = {};
		quadParams.position = m_bboxKey.GetCenterPoint() + Vector3f(0, 0, 2);
		quadParams.size = Vector3f(1, 1, 1) * (m_bboxKey.GetExtents().y - m_nodeRenderer->GetPadding().x * 0.5F);
		quadParams.uvs = Rect(128.0F / 1024, 0.0F, 128.0F / 1024, 128.0F / 1024);
		quadParams.texture = &m_nodeRenderer->GetRenderResources().m_uiElementsTexture;
		quadParams.color = Color(1, 1, 1, 1);
		buildQuad(quadParams);
	}
}

void m04::editor::sequence::BooleanPropertyRenderer::UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox )
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
		upper_left_corner + Vector3f(left_column_width, 0, 0) + Vector3f(m_nodeRenderer->GetPadding().x + ui::eventide::DefaultStyler.text.buttonSize, -ui::eventide::DefaultStyler.text.buttonSize, 4.0F)
	);
}