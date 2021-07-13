#include "./InvalidPropertyRenderer.h"
#include "./NodeBoardRenderer.h"

/*void m04::editor::sequence::InvalidPropertyRenderer::OnClicked ( const ui::eventide::Element::EventMouse& mouse_event )
{
	typedef ui::eventide::Element::EventMouse EventMouse;

	ARCORE_ASSERT(mouse_event.type == EventMouse::Type::kClicked);

	if (mouse_event.button == core::kMBLeft)
	{
		if (m_bboxKey.IsPointInBox(mouse_event.position_world))
		{
			GetNode()->view->SetProperty(m_property->identifier, !GetNode()->view->GetPropertyAsBool(m_property->identifier));
		}
	}
}*/

void m04::editor::sequence::InvalidPropertyRenderer::BuildMesh ( void )
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

	textParams = ParamsForText();
	textParams.string = "!!";
	textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
	textParams.position = m_bboxKey.GetCenterPoint() - Vector3f(m_bboxKey.GetExtents().x, m_bboxKey.GetExtents().y, 0) + Vector3f(-2, 0, 0.1F);
	textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kRight;
	textParams.color = Color(1, 0, 0, 1.0);
	buildText(textParams);

	const char* str = GetNode()->view->GetPropertyAsString(m_property->identifier);
	textParams = ParamsForText();
	textParams.string = str;
	textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
	textParams.position = m_bboxKey.GetCenterPoint() - Vector3f(m_bboxKey.GetExtents().x, m_bboxKey.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F);
	buildText(textParams);
}

void m04::editor::sequence::InvalidPropertyRenderer::UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox )
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