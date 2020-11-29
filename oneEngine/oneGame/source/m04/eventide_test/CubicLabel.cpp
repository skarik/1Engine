#include "CubicLabel.h"
#include "m04/eventide/UserInterface.h"

ETCubicLabel::ETCubicLabel ( ui::eventide::UserInterface* ui )
	: Element(ui)
{
	m_fontTexture = LoadTextureFont("YanoneKaffeesatz-B.otf");
}

ETCubicLabel::~ETCubicLabel ( void )
{
	ReleaseTexture(m_fontTexture);
}

void ETCubicLabel::BuildMesh ( void )
{
	ParamsForCube cubeParams;
	cubeParams.color = GetMouseInside() ? Color(0.2, 0.2, 0.5, 1.0) : Color(0.2, 0.2, 0.2, 1.0);
	cubeParams.box = core::math::Cubic(GetBBoxAbsolute().GetCenterPoint(), GetBBoxAbsolute().GetExtents());
	buildCube(cubeParams);

	ParamsForText textParams;
	textParams.string = "Hello world!";
	textParams.font_texture = &m_fontTexture;
	textParams.position = GetBBoxAbsolute().GetCenterPoint();
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