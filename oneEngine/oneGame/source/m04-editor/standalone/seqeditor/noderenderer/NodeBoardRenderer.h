#ifndef M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_RENDERER_H_
#define M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_RENDERER_H_

#include "../NodeBoardState.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"

namespace m04 {
namespace editor {
namespace sequence {
	
	class NodeBoardState;
	class IPropertyRenderer;
	struct PropertyRendererCreateParams;

	struct DragState
	{
		enum class Target
		{
			kNone,

			kFlowInput,
			kFlowOutput,
			kFlowSync,

			kLogicInput,
			kLogicOutput,

			kPropertyValue,
		};

		// Are we currently dragging
		bool				active = false;
		// What we started dragging from
		Target				dragStart = Target::kNone;
		// Index of the item we're dragging
		uint32_t			index = 0;
	};

	struct NodeDisplayInfo
	{
		Color				color;
	};

	IPropertyRenderer*	CreatePropertyRenderer ( m04::editor::PropertyRenderStyle propertyType, const PropertyRendererCreateParams& params );

	// less renderer, more an interactable
	class NodeRenderer : public m04::editor::sequence::INodeDisplay, public ui::eventide::elements::Button
	{
	public:
		explicit				NodeRenderer (m04::editor::sequence::NodeBoardState* in_nbs, m04::editor::sequence::BoardNode* in_node, ui::eventide::UserInterface* ui);
	protected:
		virtual					~NodeRenderer ( void );

	public:
		virtual void			OnEventMouse ( const EventMouse& mouse_event ) override;
		//	BuildMesh() : Rebuilds the mesh for Eventide UI.
		// This controls all the visual layouts and logic for the renderer.
		virtual void			BuildMesh ( void ) override;
	private:
		void					BuildMeshPropertyBoolean ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition );
		void					BuildMeshPropertyFloat ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition );
		void					BuildMeshPropertyScriptText ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition );
		void					BuildMeshPropertyEnumDropdown ( const Vector3f& in_nodeTopLeft, const m04::editor::SequenceViewProperty& in_property, const uint32_t in_propertyIndex, Vector3f& inout_penPosition );
	public:
		virtual void			OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

		//	OnDoneLoading() : Overloadable for calling loading done.
		virtual void			OnDoneLoading ( void ) override;

		void					UpdateCachedVisualInfo ( void );
		void					UpdateNextNode ( void );
		void					UpdatePropertyLayout ( void );
		void					UpdateHalfsize ( void );
		void					UpdateBboxSize ( void );
		void					UpdateBbox ( void );

		const std::vector<m04::editor::sequence::INodeDisplay*>&
								GetNextNodes ( void ) const
			{ return m_nextNodes; }

		m04::editor::sequence::INodeDisplay*
								GetNextNode ( const uint index ) const
			{ return (index < m_nextNodes.size()) ? m_nextNodes[index] : nullptr; }

	protected:

		void					OnClicked ( const EventMouse& mouse_event );
		void					OnReleased ( const EventMouse& mouse_event );

	public:

		core::math::BoundingBox	GetBboxFlowInput ( void );
		core::math::BoundingBox GetBboxFlowOutput ( const uint32_t output_index );

		enum class PropertyComponent
		{
			All,
			Key,
		};
		template <PropertyComponent Part>
		core::math::BoundingBox GetBboxProperty ( const uint32_t property_index );

		Real					GetBboxOfAllProperties ( void );

	public:
		// State of each property
		struct PropertyState
		{
			bool			m_editing = false;
			bool			m_hovered = false;
		};

		// All resources used for building mesh or rendering
		struct RenderResources
		{
			// Font texture
			ui::eventide::Texture
								m_fontTexture;
			// Scripting texture
			ui::eventide::Texture
								m_fontTextureScripting;
			// GUI texture.
			ui::eventide::Texture
								m_uiElementsTexture;
		};

		RenderResources&		GetRenderResources ( void )
			{ return m_renderResources; }
		const Vector3f&			GetMargins ( void )
			{ return m_margins; }
		const Vector3f&			GetPadding ( void )
			{ return m_padding; }

		NodeBoardState*			GetNodeBoardState ( void ) const
			{ return m_board; }

	protected:
		Vector3f			m_halfsizeOnBoard;
		Vector3f			m_margins;
		Vector3f			m_padding;
		Vector3f			m_connectionHalfsize;

	private:
		// Board state this renderer is associated with.
		NodeBoardState*		m_board = NULL;

		// Currently selected?
		bool				m_selected = false;

		// Currently being dragged?
		bool				m_dragging = false;
		// Starting Bbox for dragging
		core::math::BoundingBox
							m_draggingStart;
		// Info for dragging a subelement
		DragState			m_draggingInfo;

		// Property hover state
		std::vector<PropertyState>
							m_propertyState;

		// Property renderers
		std::vector<IPropertyRenderer*>
							m_propertyRenderers;

		RenderResources		m_renderResources;

		// Nodes currently selected as the "next" nodes in the flow. Corresponds to node->nextNodes.
		std::vector<m04::editor::sequence::INodeDisplay*>
							m_nextNodes;
		// Display name of the node
		arstring128			m_display_text;
		// GUID of the node, cached for rendering.
		arstring128			m_guid_text;
		// Color tint of the node
		Color				m_display_tint;

	private:
		// Local bbox for flow the first flow input
		core::math::BoundingBox
							m_bbox_flow_input;

		// Local bbox for the first flow output
		core::math::BoundingBox
							m_bbox_flow_output;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_RENDERER_H_