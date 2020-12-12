#include "NodeBoardState.h"
#include "m04/eventide/elements/DefaultStyler.h"
#include "m04/eventide/elements/Button.h"

#include "./SequenceEditor.h"

class NodeRenderer : public m04::editor::sequence::INodeDisplay, public ui::eventide::elements::Button
{
public:
	explicit				NodeRenderer (m04::editor::sequence::BoardNode* in_node, ui::eventide::UserInterface* ui)
		: ui::eventide::elements::Button(ui)
		, m04::editor::sequence::INodeDisplay(in_node)
		, m_halfsizeOnBoard(Vector3f(40, 20, 5))
	{
		//m_fontTexture = LoadTextureFont(ui::eventide::DefaultStyler.text.font);

		m_mouseInteract = MouseInteract::kCapturing;
		m_frameUpdate = FrameUpdate::kPerFrame;

		SetBBox(core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard));
	}

	virtual					~NodeRenderer ( void )
	{
		//ReleaseTexture(m_fontTexture);
	}

	virtual void			OnEventMouse ( const EventMouse& mouse_event ) override
	{
		if (mouse_event.type == EventMouse::Type::kDragged)
		{
			core::math::BoundingBox bbox = GetBBox();
			bbox.m_M.translate(mouse_event.velocity_world);
			bbox.m_MInverse = bbox.m_M.inverse();

			node->position = bbox.GetCenterPoint() - m_halfsizeOnBoard;

			SetBBox( bbox );
			RequestUpdateMesh();
		}
		else
		{
			if (mouse_event.type == EventMouse::Type::kClicked)
			{
				m_dragging = true;
				m_ui->LockMouse();
			}
			else if (mouse_event.type == EventMouse::Type::kReleased)
			{
				m_dragging = false;
				m_ui->UnlockMouse();
			}

			Button::OnEventMouse(mouse_event);
		}
	}

	virtual void			BuildMesh ( void ) override
	{
		using namespace ui::eventide;

		ParamsForCube cubeParams;
		cubeParams.box = core::math::Cubic::ConstructFromBBox(GetBBoxAbsolute());
		cubeParams.rotation = GetBBoxAbsolute().m_M.getRotator();
		cubeParams.texture = NULL;
		cubeParams.color = DefaultStyler.box.defaultColor
			.Lerp(DefaultStyler.box.hoverColor, m_hoverGlowValue)
			.Lerp(DefaultStyler.box.activeColor, Styler::PulseFade(m_activateGlowPulse));
		buildCube(cubeParams);

		ParamsForText textParams;
		textParams.string = "Default node";
		textParams.font_texture = &m_fontTexture;
		textParams.position = GetBBoxAbsolute().GetCenterPoint() - GetBBoxAbsolute().GetExtents() + Vector3f(0, 0, GetBBoxAbsolute().GetExtents().z * 2.0F + 1.0F);
		textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
		textParams.size = ui::eventide::DefaultStyler.text.headingSize;
		textParams.color = DefaultStyler.text.headingColor;
		buildText(textParams);
	}

	virtual void			OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override
	{
		core::math::BoundingBox bbox = GetBBox();

		bbox = core::math::BoundingBox(Rotator(), node->position + m_halfsizeOnBoard, m_halfsizeOnBoard);

		SetBBox(bbox);
		RequestUpdateMesh();

		Button::OnGameFrameUpdate(input_frame);

		m_frameUpdate = FrameUpdate::kPerFrame;
	}

protected:
	Vector3f			m_halfsizeOnBoard;

private:
	//ui::eventide::Texture
	//					m_fontTexture;

	bool				m_dragging = false;
};

m04::editor::sequence::NodeBoardState::NodeBoardState ( m04::editor::SequenceEditor* editor )
	: ui( editor->GetEventideUI() )
{
	;
}

//	AddDisplayNode( board_node ) : Adds node to display. Allocates and sets up a proper display object.
void m04::editor::sequence::NodeBoardState::AddDisplayNode ( BoardNode* board_node )
{
	board_node->display = new NodeRenderer(board_node, ui);

	// Add both node and node's display
	nodes.push_back(board_node);
	display.push_back(board_node->display);
}