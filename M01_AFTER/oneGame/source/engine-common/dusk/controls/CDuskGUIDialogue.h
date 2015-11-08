
#ifndef _C_DUSK_GUI_DIALOGUE_
#define _C_DUSK_GUI_DIALOGUE_

#include "CDuskGUIPanel.h"
#include <vector>
//using std::vector;

class CDuskGUIButton;

class CDuskGUIDialogue : public CDuskGUIPanel
{
public:
	CDuskGUIDialogue ( void ) : dialogueReturn(-1), CDuskGUIPanel(31) { ; };
	CDuskGUIDialogue ( const int typeoverride ) : dialogueReturn(-1), CDuskGUIPanel(typeoverride) { ; };
	~CDuskGUIDialogue ( void );

	// Overridable update
	void Update ( void );
	void Render ( void );

public:
	// Button state
	bool	inDialogueMode;

	int		dialogueReturn;

	std::vector<CDuskGUIButton*> buttonList;
};

#endif