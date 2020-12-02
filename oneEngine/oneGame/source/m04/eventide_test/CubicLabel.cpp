#include "CubicLabel.h"
#include "m04/eventide/UserInterface.h"
#include "m04/eventide/elements/DefaultStyler.h"

ETCubicLabel::ETCubicLabel ( ui::eventide::UserInterface* ui )
	: Element(ui)
{
	m_fontTexture = LoadTextureFont(ui::eventide::DefaultStyler.text.font);

	m_mouseInteract = MouseInteract::kCapturing;
}

ETCubicLabel::~ETCubicLabel ( void )
{
	ReleaseTexture(m_fontTexture);
}

void ETCubicLabel::BuildMesh ( void )
{
	ParamsForCube cubeParams;
	cubeParams.color = GetMouseInside() ? ui::eventide::DefaultStyler.box.hoverColor : ui::eventide::DefaultStyler.box.defaultColor;
	cubeParams.box = core::math::Cubic::ConstructCenterExtents(GetBBoxAbsolute().GetCenterPoint(), GetBBoxAbsolute().GetExtents());
	buildCube(cubeParams);

	ParamsForText textParams;
	textParams.string = "Hello world!";
	textParams.font_texture = &m_fontTexture;
	textParams.position = GetBBoxAbsolute().GetCenterPoint() + Vector3f(0, 0, 32);
	textParams.size = 24.0F;
	buildText(textParams);
}

void ETCubicLabel::OnEventMouse ( const EventMouse& mouse_event )
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
}