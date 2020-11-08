#include "CubicLabel.h"

ETCubicLabel::ETCubicLabel ( ui::eventide::UserInterface* ui )
	: Element(ui)
{
}

ETCubicLabel::~ETCubicLabel ( void )
{
	;
}

void ETCubicLabel::BuildMesh ( void )
{
	ParamsForCube cubeParams;
	cubeParams.color = m_mouseInside ? Color(0.2, 0.2, 0.5, 1.0) : Color(0.2, 0.2, 0.2, 1.0);
	buildCube(cubeParams);

	ParamsForText textParams;
	textParams.string = "Hello world!";
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