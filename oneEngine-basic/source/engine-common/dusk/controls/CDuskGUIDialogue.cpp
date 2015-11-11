
#include "../CDuskGUI.h"
#include "CDuskGUIButton.h"
#include "CDuskGUIDialogue.h"

void CDuskGUIDialogue::Update ( void )
{
	CDuskGUIElement::Update();
	if ( inDialogueMode )
	{
		for ( unsigned int i = 0; i < buttonList.size(); ++i ) {
			buttonList[i]->rect.pos = rect.pos + Vector2d( rect.size.x*0.05f + rect.size.x*0.2f*i, rect.size.y*0.75f );
			buttonList[i]->rect.size = Vector2d( rect.size.x * 0.12f, rect.size.y * 0.21f );
			buttonList[i]->Update();
		}
	}

	if ( inDialogueMode )
	{
		visible = true;
		hasFocus = true;

		activeGUI->hCurrentDialogue = activeGUI->hCurrentElement;
		// Check buttons
		for ( unsigned int i = 0; i < buttonList.size(); ++i )
		{
			if ( buttonList[i]->isPressed )
			{
				dialogueReturn = i;
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
			}
		}
	}
	else
	{
		visible = false;
	}
}
void CDuskGUIDialogue::Render ( void )
{
	// Begin draw/ material
	drawRectWire( rect );
	drawRect( rect );

	// Now draw text
	drawTextWidth( rect.pos.x + rect.size.x*0.1f, rect.pos.y + rect.size.y*0.1f  + 0.02f, rect.size.x*0.87f, label.c_str() );

	for ( unsigned int i = 0; i < buttonList.size(); ++i )
	{
		buttonList[i]->Render();
	}
}

CDuskGUIDialogue::~CDuskGUIDialogue ( void )
{
	for ( unsigned int i = 0; i < buttonList.size(); ++i )
	{
		delete buttonList[i];
		buttonList[i] = NULL;
	}
	buttonList.clear();
}