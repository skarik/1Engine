#ifndef M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_RENDERER_H_
#define M04_EDITORS_SEQUENCE_EDITOR_NODE_BOARD_RENDERER_H_

#include "NodeBoardState.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"

namespace m04 {
namespace editor {
namespace sequence {
	
	class NodeBoardState;

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

		bool					active = false;
		Target					target = Target::kNone;
		uint32_t				index = 0;
	};

	// less renderer, more an interactable
	class NodeRenderer : public m04::editor::sequence::INodeDisplay, public ui::eventide::elements::Button
	{
	public:
		explicit				NodeRenderer (m04::editor::sequence::NodeBoardState* in_nbs, m04::editor::sequence::BoardNode* in_node, ui::eventide::UserInterface* ui);
		virtual					~NodeRenderer ( void );

		virtual void			OnEventMouse ( const EventMouse& mouse_event ) override;
		virtual void			BuildMesh ( void ) override;
		virtual void			OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

		void					UpdateNextNode ( void );

	protected:

		void					OnClicked ( const EventMouse& mouse_event );
		void					OnReleased ( const EventMouse& mouse_event );

		core::math::BoundingBox	GetBboxFlowInput ( void );
		core::math::BoundingBox GetBboxFlowOutput ( const uint32_t output_index );

	protected:
		Vector3f			m_halfsizeOnBoard;
		Vector3f			m_margins;
		Vector3f			m_connectionHalfsize;

	private:
		// Board state this renderer is associated with.
		NodeBoardState*		m_board = NULL;

		// Currently being dragged?
		bool				m_dragging = false;
		// Info for dragging a subelement
		DragState			m_draggingInfo;

		// GUI texture.
		ui::eventide::Texture
							m_uiElementsTexture;

		// Node currently selected as the "next" node in the flow. Corresponds to node->next.
		m04::editor::sequence::INodeDisplay*
							m_next;
		// Display name of the node
		arstring128			m_display_text;
		// GUID of the node, cached for rendering.
		arstring<9>			m_guid_text;

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