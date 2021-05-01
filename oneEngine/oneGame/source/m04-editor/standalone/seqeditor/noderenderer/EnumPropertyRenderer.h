#ifndef M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_ENUM_H_
#define M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_ENUM_H_

#include "./IPropertyRenderer.h"
#include "core-ext/containers/arStringEnum.h"

namespace m04 {
namespace editor {
namespace sequence {

	class EnumPropertyRenderer : public IPropertyRenderer
	{
	public:
		explicit				EnumPropertyRenderer ( const CreationParameters& params );

	public:

		virtual void			BuildMesh ( void ) override;
		virtual void			OnClicked ( const ui::eventide::Element::EventMouse& mouse_event ) override;

		virtual void			UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox ) override;

	private:
		arStringEnumDefinition*
							m_enumDefinition = nullptr;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_FLOAT_H_
