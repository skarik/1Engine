#include "Label.h"
#include "m04/eventide/elements/DefaultStyler.h"

ui::eventide::elements::Label::Label ( ui::eventide::UserInterface* ui )
	: Element(ui)
{
	m_fontTexture = LoadTextureFont(ui::eventide::DefaultStyler.text.font);
	m_mouseInteract = MouseInteract::kNone;
}

ui::eventide::elements::Label::~Label ( void )
{
	ReleaseTexture(m_fontTexture);
}

void ui::eventide::elements::Label::BuildMesh ( void )
{
	ParamsForText textParams;
	textParams.string = m_contents.c_str();
	textParams.font_texture = &m_fontTexture;
	textParams.position = GetBBoxAbsolute().GetCenterPoint() - GetBBoxAbsolute().GetExtents();
	textParams.rotation = GetBBoxAbsolute().m_M.getRotator();
	textParams.size = ui::eventide::DefaultStyler.text.headingSize;
	buildText(textParams);
}