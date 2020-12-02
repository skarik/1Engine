#include "Button.h"
#include "m04/eventide/elements/DefaultStyler.h"

#include "core/time.h"

ui::eventide::elements::Button::Button ( ui::eventide::UserInterface* ui )
	: Element(ui)
{
	m_fontTexture = LoadTextureFont(ui::eventide::DefaultStyler.text.font);
	m_mouseInteract = MouseInteract::kCapturing;
}

ui::eventide::elements::Button::~Button ( void )
{
	ReleaseTexture(m_fontTexture);
}

void ui::eventide::elements::Button::BuildMesh ( void )
{
	ParamsForCube cubeParams;
	cubeParams.box = core::math::Cubic::ConstructFromBBox(GetBBoxAbsolute());
	cubeParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	cubeParams.texture = NULL;
	cubeParams.color = (GetMouseInside() ? DefaultStyler.box.hoverColor : DefaultStyler.box.defaultColor).Lerp(DefaultStyler.box.activeColor, m_activateGlowPulse);
	buildCube(cubeParams);

	ParamsForText textParams;
	textParams.string = m_contents.c_str();
	textParams.font_texture = &m_fontTexture;
	textParams.position = GetBBoxAbsolute().GetCenterPoint() - GetBBoxAbsolute().GetExtents() + Vector3f(0, 0, GetBBoxAbsolute().GetExtents().z * 2.0F + 1.0F);
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = ui::eventide::DefaultStyler.text.buttonSize;
	textParams.color = m_enabled ? DefaultStyler.text.buttonColor : DefaultStyler.text.buttonDisabledColor;
	buildText(textParams);
}

void ui::eventide::elements::Button::SetEnabled ( bool enable )
{
	if (m_enabled != enable)
	{
		RequestUpdateMesh();
		m_enabled = enable;
		m_mouseInteract = m_enabled ? MouseInteract::kCapturing : MouseInteract::kNone;
	}
}

void ui::eventide::elements::Button::OnEventMouse ( const EventMouse& mouse_event )
{
	if (m_enabled)
	{
		// Update visuals on mouse in/out
		if (mouse_event.type == EventMouse::Type::kEnter)
		{
			RequestUpdateMesh();
		}
		else if (mouse_event.type == EventMouse::Type::kExit)
		{
			RequestUpdateMesh();
		}
		// Do a flash when clicked
		else if (mouse_event.type == EventMouse::Type::kClicked)
		{
			RequestUpdateMesh();
			m_frameUpdate = FrameUpdate::kPerFrame;
			m_activateGlowPulse = 1.0F;
			OnActivated();
		}
	}
}

void ui::eventide::elements::Button::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	m_activateGlowPulse = m_activateGlowPulse - Time::deltaTime / 0.5F;
	if (m_activateGlowPulse <= 0.0F)
	{
		m_frameUpdate = FrameUpdate::kNone;
	}
	else
	{
		RequestUpdateMesh();
	}
}