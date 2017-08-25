#ifndef _C_DIALOGUE_LOADER_H_
#define _C_DIALOGUE_LOADER_H_

#include "core-ext/system/io/mccosf.h"
#include <string>
#include <vector>
//#include "after/entities/character/CCharacter.h"

using std::string;
class CActor;

enum DialogueState_enum
{
	DIALOGUE_STATE_PLAYERTALKING = 0,
	DIALOGUE_STATE_NPCTALKING,
	DIALOGUE_STATE_CHOICES,
	DIALOGUE_STATE_RESPONSE,
	DIALOGUE_STATE_LUACODE,
	DIALOGUE_STATE_ADDRESS,
	DIALOGUE_STATE_ENDED
};

class CDialogueLoader
{
public:
	struct ChoiceStruct
	{
		string choice;
		string address;
	};
	struct LineType
	{
		string line;
		string audio;
		string action;
		string expression;
	};

public:
	explicit CDialogueLoader (const string& filename);
	explicit CDialogueLoader (const char* filename);

	explicit CDialogueLoader (const string& filename, std::vector<CActor*> characters);
	explicit CDialogueLoader (const char* filename, std::vector<CActor*> characters);

private:
	void Construct ( const char* filename, std::vector<CActor*> characters );

public:
	~CDialogueLoader (void);
	
	string GetCurrentLine(void);
	string GetCurrentAudio(void);
	string GetCurrentAction(void);
	string GetCurrentExpression(void);
	string GetCurrentAddress(void);

	void GoNextLine (void);

	std::vector<ChoiceStruct> GetChoices (void);

	char* GetLua (void);

	void ReportDecision (short choice);
	bool IsTimedChoice ( void ) const;
	float CurrentChoiceTimer ( void ) const;
	float GetTimerLength (void) const;
	short GetDefaultChoice ( void ) const;

	void SkipTo (ChoiceStruct selected);

	// Choice update step
	void ChoiceUpdate ( void );
	// Lua update step
	void PostLua (void);
	// Dialogue action/face update step
	void SoundAndFace (void);	

	// Return the index of the current speaker. May not be valid.
	short GetCurrentSpeaker ( void ) const;

	// Return the numbered participant of the conversation. Typically used for event hooks and name rendering.
	CActor* GetParticipant ( const short );

	//Current state of dialogue, to keep track of what the dialogue GUI should be looking for
	short sDialogueState;
private:
	FILE* pFile;
	COSF_Loader* lCurrentDialogue;
	mccOSF_entry_info_t sCurrentEntry;
	//File name that we're currently working with
	string mDialogue;
	//Where the dialogue is at in the file
	string sIndex;
	//The line that should be spoken. 
	string sCurrentLine;
	//The set of choices for the player
	std::vector<ChoiceStruct>			sOptions;
	float				m_choiceTimer;
	float				m_choiceTimerSet;
	bool				m_choiceTimerActive;
	short				m_choiceDefault;
	std::vector<LineType>				sLines;

	// Lua code
	static char m_luaCode [2048];
	int			m_luaState;

	short	LineIt;
	
	void NextObject (void);
	
	void MakeLines (void);
	void MakeChoices (void);
	void MakeLua (void);
	
	string sPrevious;

	std::vector<CActor*>	nCharacters;
	short				sTalker;
};

#endif