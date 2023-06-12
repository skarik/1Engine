#ifndef M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_H_
#define M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_H_

#include "m04/eventide/Element.h"
#include "./NodeBoardRenderer.h"

namespace m04 {
namespace editor {
namespace sequence {

	class NodeRenderer;

	struct PropertyRendererCreateParams
	{
		NodeRenderer*		node_renderer;
		const m04::editor::SequenceViewProperty*
							property;
		NodeRenderer::PropertyState*
							property_state;
		osf::ObjectValue*	target_data = nullptr;
	};

	// TODO: may want to move the state outside of the renderer, so that we don't need to instantiate renderers, just use em as a "view" of sorts
	// m_nodeRenderer, m_property, and m_propertyState are all owned by someone else anyways...

	class IPropertyRenderer : public ui::eventide::Element::RenderContext
	{
	public:
		explicit				IPropertyRenderer ( const PropertyRendererCreateParams& params )
			: ui::eventide::Element::RenderContext(params.node_renderer)
			, m_nodeRenderer(params.node_renderer)
			, m_propertyState(params.property_state)
			, m_property(params.property)
			, m_targetData(params.target_data)
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

		virtual void			BuildMesh ( void ) {}
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
		osf::ObjectValue*	m_targetData = nullptr;

		Real				m_bboxHeight;
		core::math::BoundingBox
							m_bboxAll;
		core::math::BoundingBox
							m_bboxKey;
	};

	//===============================================================================================//

	// Data-forward property visualizer prototyping

	typedef PropertyRendererCreateParams PropertyVisualizationData;
	struct PropertyVisualizationState
	{
		// Hover state of the property
		NodeRenderer::PropertyState
							m_propertyState;
		// Bounding box of the property
		Real				m_bboxHeight;
		core::math::BoundingBox
							m_bboxAll;
		core::math::BoundingBox
							m_bboxKey;
	};

	struct PropertyVisualizationInput
	{
		PropertyVisualizationData*	data;
		PropertyVisualizationState*	state;
	};

	typedef void (*PropertyVisBuildMesh) ( PropertyVisualizationInput& );
	typedef void (*PropertyVisOnClicked) ( PropertyVisualizationInput&, const ui::eventide::Element::EventMouse& mouse_event );
	typedef void (*PropertyVisOnGameFrameUpdate) ( PropertyVisualizationInput&, const ui::eventide::Element::GameFrameUpdateInput& input_frame );
	typedef void (*PropertyVisUpdateLayout) ( PropertyVisualizationInput&, const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox );

	struct PropertyVisualization
	{
		// Called when node needs to update layout. Should be called before BuildMesh() but is not gauranteed.
		PropertyVisUpdateLayout
							UpdateLayout = nullptr;
		// Called for each property when building the mesh
		PropertyVisBuildMesh
							BuildMesh = nullptr;

		// Behavior when the property is clicked on
		PropertyVisOnClicked
							OnClicked = nullptr;
		// Behavior on each frame the property is being rendered
		PropertyVisOnGameFrameUpdate
							OnGameFrameUpdate = nullptr;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_PROPERTY_RENDERER_H_