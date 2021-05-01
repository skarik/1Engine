#ifndef M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_H_
#define M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_H_

#include "m04/eventide/Element.h"
#include "./NodeBoardRenderer.h"

namespace m04 {
namespace editor {
namespace sequence {

	class NodeRenderer;

	class IPropertyRenderer : public ui::eventide::Element::RenderContext
	{
	public:
		struct CreationParameters
		{
			NodeRenderer* node_renderer;
			const m04::editor::SequenceViewProperty* property;
			NodeRenderer::PropertyState* property_state;
		};

		explicit				IPropertyRenderer ( const CreationParameters& params )
			: ui::eventide::Element::RenderContext(params.node_renderer)
			, m_nodeRenderer(params.node_renderer)
			, m_propertyState(params.property_state)
			, m_property(params.property)
			{}

		virtual					~IPropertyRenderer ( void )
			{}

	protected:

		m04::editor::SequenceNode*
								GetNode ( void ) const
		{
			return m_nodeRenderer->GetBoardNode()->sequenceInfo;
		}

	public:

		virtual void			BuildMesh ( Vector3f& inout_penPosition ) {}
		virtual void			OnClicked ( const ui::eventide::Element::EventMouse& mouse_event ) {}
		virtual void			OnGameFrameUpdate ( const ui::eventide::Element::GameFrameUpdateInput& input_frame ) {}

		//	virtual UpdateLayout() : Called when layout needs to be updated (when the node size or position changes)
		virtual void			UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox ) {}

		//	GetCachedBboxHeight() : Return cached bounding box height.
		const Real				GetCachedBboxHeight ( void )
			{ return m_bboxHeight; }
		//	GetCachedBboxAll() : Return cached bounding box in world-space of the element.
		const core::math::BoundingBox&
								GetCachedBboxAll ( void )
			{ return m_bboxAll; }
		//	GetCachedBboxAll() : Return cached bounding box in world-space of the element's key area.
		const core::math::BoundingBox&
								GetCachedBboxKey ( void )
			{ return m_bboxKey; }

	protected:
		NodeRenderer*		m_nodeRenderer = nullptr;
		const m04::editor::SequenceViewProperty*
							m_property = nullptr;
		NodeRenderer::PropertyState*
							m_propertyState = nullptr;

		Real				m_bboxHeight;
		core::math::BoundingBox
							m_bboxAll;
		core::math::BoundingBox
							m_bboxKey;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_H_