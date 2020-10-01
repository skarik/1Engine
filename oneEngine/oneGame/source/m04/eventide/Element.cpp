#include "Element.h"

#include "../eventide/UserInterface.h"

ui::eventide::Element::Element ( UserInterface* ui )
	: m_ui((ui != NULL) ? ui : ui::eventide::UserInterface::Get())
{
	m_ui->AddElement(this);
}

ui::eventide::Element::~Element ( void )
{
	m_ui->RemoveElement(this);
}