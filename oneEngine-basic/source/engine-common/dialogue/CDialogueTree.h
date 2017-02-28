#pragma once
//#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>
#include "CDialogueLoader.h"

enum NODE_TYPE
{
	DIALOGUE_NULL = 0,
	DIALOGUE_LINE,
	DIALOGUE_CHOICE,
	DIALOGUE_WAIT,
	DIALOGUE_END
};

struct DialogueNode
{
	int ID;
	int type; //Dialogue or Choice, really
	int flags;
	int length;
	int character;
	int Parent; //Index of the parent node for backtracking
	std::vector<int> Next; //Contains the indexes of the children nodes.
	std::vector<std::string> Lines;
	std::string motion;
	std::string sound;
	char address[256]; //Only used if this is a branch
};

struct ChoiceNode : DialogueNode
{
	std::vector<CDialogueLoader::ChoiceStruct> Choices;
	int defaultChoice;
};

class CDialogueTree
{
public:
	CDialogueTree (void);
	CDialogueTree (char *file);
	~CDialogueTree (void);

	/*Functions related to advancing the dialogue*/

	//Go to the next line of dialogue
	void AdvanceDialogue (void);
	//Report the player's choice. Should be 0-indexed. -1 or (void) to use default choice
	int ReportChoice (int choice = -1);
	//Go back one line of dialogue
	int Backtrack (void);

	/*Functions for getting data about the current line of dialogue*/

	//Get the line as a C-string
	const char *GetLine (void);
	//Get the choices
	std::vector<string> GetChoices (void);
	//Get the flags
	int GetFlags (void);
	//Get the length
	int GetLength (void);
	
	/*GetCharacter (void);
	GetMotionData (void);
	GetSoundData (void);*/

	//Prints the contents of the tree to cout
	void PrintTree (void);

	NODE_TYPE sNodeType;

private:
	void CreateTree (char * file);
	void CreateNode (void);
	void DestroyTree (void);
	void CreateLinks (void);

	CDialogueLoader *pLoader;

	/*DialogueNode *mRoot; 
	DialogueNode *mCurrent;*/

	//Root is obviously position 0
	std::vector<DialogueNode *> mTree; //Store all the nodes in a vector for easy management
	int mCurrent; //index of current line of dialogue
};