#ifndef M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_SCRIPT_TEXT_H_
#define M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_SCRIPT_TEXT_H_

#include "./IPropertyRenderer.h"

namespace m04 {
namespace editor {
namespace sequence {

	class ScriptTextPropertyRenderer : public IPropertyRenderer
	{
	public:
		explicit				ScriptTextPropertyRenderer ( const PropertyRendererCreateParams& params )
			: IPropertyRenderer(params)
			{}

	public:

		virtual void			BuildMesh ( void ) override;
		virtual void			OnClicked ( const ui::eventide::Element::EventMouse& mouse_event ) override;
		virtual void			OnGameFrameUpdate ( const ui::eventide::Element::GameFrameUpdateInput& input_frame ) override;

		virtual void			UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox ) override;

	private:
		// Font size we render at
		Real				m_fontSize = 10.0F;

		// Number of lines the output is, for sizing the layout
		int32_t				m_lineCount = 0;
		// Current position of the editing cursor.
		int32_t				m_cursorPosition = 0;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_SCRIPT_TEXT_H
