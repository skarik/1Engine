#ifndef _C_DIALOGUE_GUI_H_
#define _C_DIALOGUE_GUI_H_

#include <vector>

#include "core/math/Vector2d.h"
#include "core/math/Rect.h"

#include "engine/behavior/CGameBehavior.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/texture/CBitmapFont.h"

#include "after/entities/character/CCharacter.h"

#include "CDialogueLoader.h"

class CInputControl;
class CPlayer;

class CDialogueGUI: public CGameBehavior, public CRenderableObject
{

	ClassName ("DialogueGUI");
public:
	CDialogueGUI (void);
	~CDialogueGUI (void);

	void Update (void);

	void LateUpdate (void);
	
	bool Render (const char pass);

	void GetMouseInfo (void);

	void OpenConversation (string filename);

	void OpenConversation (string filename, std::vector<CCharacter*> characters);

	void SetActive (bool active);

	bool GetIsActive (void);

private:

	void DrawDialogue (void);
	void DrawChoices (void);
	void DrawBackground (void);
	void DrawHighlighted (void);
	
	bool				bActive;
	CDialogueLoader*	mConversation;
	float				sMouseX;
	float				sMouseY;
	float				xCenter;
	short				sChoice;
	short				sNumChoices;
	std::vector<Vector2d>	rPositions;
	Vector2d			rSize;
	//Vector2d			vBackgroundLineP;
	//Vector2d			vBackgroundLineS;
	Rect				mBackgroundDialogue;
	//Vector2d			vBackgroundChoicesP;
	//Vector2d			vBackgroundChoicesS;
	Rect				mBackgroundChoices;

	std::vector<CDialogueLoader::ChoiceStruct> choices;

	//Use for Lua
	//static char cCode [2048];

	CInputControl*		input;
	
	glMaterial*		matFntDebug;
	CBitmapFont*	fntDebug;
	glMaterial*		matDrawDebug;
};

#endif