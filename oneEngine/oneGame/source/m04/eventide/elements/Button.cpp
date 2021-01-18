#include "Button.h"
#include "m04/eventide/elements/DefaultStyler.h"

#include "core/time.h"
#include "core/math/Easing.h"

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
	cubeParams.color = DefaultStyler.box.defaultColor
		.Lerp(DefaultStyler.box.hoverColor, m_hoverGlowValue)
		.Lerp(DefaultStyler.box.activeColor, Styler::PulseFade(m_activateGlowPulse));
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
			m_frameUpdate = FrameUpdate::kPerFrame;
		}
		else if (mouse_event.type == EventMouse::Type::kExit)
		{
			RequestUpdateMesh();
			m_frameUpdate = FrameUpdate::kPerFrame;
		}
		// Do a flash when clicked
		else if (mouse_event.type == EventMouse::Type::kClicked
			&& mouse_event.button == core::kMBLeft)
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
	m_activateGlowPulse = m_activateGlowPulse - Time::deltaTime / DefaultStyler.timing.clickPulse;

	m_hoverGlowValue = math::clamp<float>(m_hoverGlowValue, 0.0F, 1.0F);
	if (GetMouseInside())
	{
		m_hoverGlowValue = m_hoverGlowValue + Time::deltaTime / DefaultStyler.timing.hoverFade;
	}
	else
	{
		m_hoverGlowValue = m_hoverGlowValue - Time::deltaTime / DefaultStyler.timing.hoverFade;
	}

	// Check that all color blends are steady, and if they are, stop updating.
	if ((GetMouseInside() ? (m_hoverGlowValue >= 1.0F) : (m_hoverGlowValue <= 0.0F))
		&& m_activateGlowPulse <= 0.0F)
	{
		m_hoverGlowValue = math::clamp<float>(m_hoverGlowValue, 0.0F, 1.0F);
		m_activateGlowPulse = std::max<float>(m_activateGlowPulse, 0.0F);
		m_frameUpdate = FrameUpdate::kNone;
	}

	RequestUpdateMesh();
}