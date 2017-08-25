
#include "../CDuskGUI.h"
#include "CDuskGUIDialogue_YN.h"
#include "CDuskGUIButton.h"

void CDuskGUIDialogue_YN::Update ( void )
{
	if ( buttonList.size() == 0 )
	{
		buttonList.push_back( new CDuskGUIButton() );
		buttonList.push_back( new CDuskGUIButton() );

		buttonList[0]->parent = activeGUI->hCurrentElement;
		buttonList[0]->label = "Yes";
		buttonList[0]->isPressed = false;

		buttonList[1]->parent = activeGUI->hCurrentElement;
		buttonList[1]->label = "No";
		buttonList[1]->isPressed = false;
	}

	// Default dialogue behavior
	CDuskGUIDialogue::Update();
}