#include "CDialogueTree.h"
#include <iostream>
#include <queue>

//Create the tree
CDialogueTree::CDialogueTree (void)
{
	//Generic tree for testing functions, I think
	sNodeType = DIALOGUE_NULL;
	mCurrent = 0;
	CreateTree ("test2.txt");
	CreateLinks();
	switch (mTree[mCurrent]->type)
	{
	case DIALOGUE_LINE:
		sNodeType = DIALOGUE_LINE;
		break;
	case DIALOGUE_CHOICE:
		sNodeType = DIALOGUE_CHOICE;
		break;
	default:
		std::cout << "How the hell did this get initialized to something other than Line or Choice?" << std::endl;
		sNodeType = DIALOGUE_LINE;
		break;
	}
	if (pLoader)
		delete pLoader;
	pLoader = NULL;
}

//Load from a given file
CDialogueTree::CDialogueTree (char * file)
{
	//Herro there
	mCurrent = 0;
	CreateTree (file);
	CreateLinks();
	switch (mTree[mCurrent]->type)
	{
	case DIALOGUE_LINE:
		sNodeType = DIALOGUE_LINE;
		break;
	case DIALOGUE_CHOICE:
		sNodeType = DIALOGUE_CHOICE;
		break;
	default:
		std::cout << "How the hell did this get initialized to something other than Line or Choice?" << std::endl;
		sNodeType = DIALOGUE_LINE;
		break;
	}

	if (pLoader)
		delete pLoader;
	pLoader = NULL;
}

CDialogueTree::~CDialogueTree (void)
{
	//Destroy everything because it's now worthless
	DestroyTree();
	/*mRoot = nullptr;
	mCurrent = nullptr;*/
}

void CDialogueTree::CreateTree (char * file)
{
	//LOAD THE DAMN THING
	pLoader = new CDialogueLoader (file);

	std::queue<CDialogueLoader::ChoiceStruct> mBranches;
	string currentAddress = ""; //Just make sure it's properly initialized
	DialogueNode *current;
	ChoiceNode *currentChoice;

	while (pLoader->sDialogueState != DialogueState_enum::DIALOGUE_STATE_ENDED)
	{
		switch (pLoader->sDialogueState)
		{
		//case DIALOGUE_STATE_LUACODE:
		//	//debug::Console->PrintMessage("Lua state. Input skipped.\n");
		//	pLoader->PostLua(); //Hopefully this is all I need to do here
		//	/*pLoader->GetLua();
		//	pLoader->GoNextLine();*/
		//	break;
		case DIALOGUE_STATE_ADDRESS:
			/*Dequeue an entry from the choices
			Set the address
			if no entries left
			return;*/
			
			if (mBranches.size() < 1) //If this is true, the entire conversation should be loaded
				return;

			currentAddress = mBranches.front().address; 
			pLoader->SkipTo(mBranches.front());
			mBranches.pop();
			pLoader->GoNextLine();
			break;
		case DIALOGUE_STATE_PLAYERTALKING:
		case DIALOGUE_STATE_NPCTALKING:
			current = new DialogueNode();
			
			current->type = DIALOGUE_LINE; //Regular dialogue line
			current->character = pLoader->GetCurrentSpeaker();
			current->ID = mTree.size();
			
			if (!currentAddress.empty())
			{
				strcpy (current->address, currentAddress.c_str());
				currentAddress.clear();
			}
			
			//What flags did I need again?
			//ignore length here
			current->Lines.push_back(pLoader->GetCurrentLine());
			current->motion = pLoader->GetCurrentExpression();
			current->sound = pLoader->GetCurrentAudio();
			current->Next.push_back(mTree.size() + 1); //Mark the next struct as the child
			current->Parent = mTree.size() - 1; //Handy trick: this makes the Parent of the first node -1 by default
			
			mTree.push_back(current);
			pLoader->GoNextLine();
			break;
		case DIALOGUE_STATE_CHOICES:
			currentChoice = new ChoiceNode();
			currentChoice->type = DIALOGUE_CHOICE; //Decision
			currentChoice->character = pLoader->GetCurrentSpeaker();
			currentChoice->ID = mTree.size();
			currentChoice->defaultChoice = pLoader->GetDefaultChoice();

			if (!currentAddress.empty())
			{
				strcpy(currentChoice->address, currentAddress.c_str());
				currentAddress.clear();
			}
			//What flags did I need again?
			//ignore length here
			currentChoice->Choices = pLoader->GetChoices();
			
			for (size_t i = 0; i < currentChoice->Choices.size(); i++)
			{
				currentChoice->Lines.push_back(currentChoice->Choices[i].choice);
				mBranches.push(currentChoice->Choices[i]);
			}

			currentChoice->motion = pLoader->GetCurrentExpression();
			currentChoice->sound = pLoader->GetCurrentAudio();
			currentChoice->Next; //Do I need this if I just store the choices and addresses anyway?
			currentChoice->Parent = mTree.size() - 1;
			
			mTree.push_back(currentChoice);
			pLoader->GoNextLine(); //Maybe? Need to figure this out.
			break;
		default:
			//debug::Console->PrintWarning("Unhandled dialogue option\n");
			std::cout << "How did we get here?" << std::endl;
			break;
		}
	}
}

//This creates the links between the choices and the branches
void CDialogueTree::CreateLinks (void)
{
	size_t index = 0;

	for (; index < mTree.size(); index++)
	{
		if (mTree[index]->type == DIALOGUE_CHOICE) //If this is a choice
		{
			ChoiceNode *current = (ChoiceNode *)(mTree[index]);

			//Make a link for each choice
			for (size_t i = 0; i < current->Choices.size(); i++)
			{
				//Search the rest of the dialogue tree for the branch
				for (size_t j = index + 1; j < mTree.size(); j++)
				{
					if (current->Choices[i].address.empty()) //Go to next node if the address for this one is empty
					{
						current->Next.push_back(index + 1);
						break;
					}
					else if (strcmp (mTree[j]->address, current->Choices[i].address.c_str()) == 0)
					{
						current->Next.push_back(j); //Set the branch destination
						if (current->Next.size() != i + 1)
							std::cout << "Error in creating links: Mismatch between index of Choices struct and number of jumps" << std::endl;
						mTree[j]->Parent = index; //Set the parent of the branch
						break;
					}
				}
			}
		}
	}
}
void CDialogueTree::CreateNode (void)
{
	std::cout << "What the fuck is this getting called for?" << std::endl;	
}

void CDialogueTree::DestroyTree (void)
{
	for (size_t i = 0; i < mTree.size(); i++)
	{
		if (mTree[i])
			free(mTree[i]);
		mTree[i] = nullptr;
	}

	mTree.clear();
}

void CDialogueTree::AdvanceDialogue (void)
{
	if (mTree[mCurrent]->type == DIALOGUE_LINE)
	{
		if (mTree[mCurrent]->Next[0] >= mTree.size() || mTree[mTree[mCurrent]->Next[0]]->Parent != mCurrent)
		{
			sNodeType = DIALOGUE_END;
			return;
		}

		mCurrent = mTree[mCurrent]->Next[0]; //Go to the next node
		switch (mTree[mCurrent]->type) //Check its type of set the state accordingly
		{
		case DIALOGUE_LINE:
			sNodeType = DIALOGUE_LINE;
			break;
		case DIALOGUE_CHOICE:
			sNodeType = DIALOGUE_CHOICE;
			break;
		default:
			std::cout << "How did I break it this time?" << std::endl;
			sNodeType = DIALOGUE_NULL;
			break;
		}
	}
	else if (mTree[mCurrent]->type == DIALOGUE_CHOICE)
		std::cout << "Error: Current node is a choice. Call ReportChoice() instead" << std::endl;
	else
		std::cout << "lolwut?" << std::endl;
}

//Choice reported should be 0-indexed.
int CDialogueTree::ReportChoice (uint choice)
{
	if ((choice < mTree[mCurrent]->Next.size() && choice >= 0) || choice == -1)
	{
		if (choice == (uint)-1)
			choice = ((ChoiceNode *)mTree[mCurrent])->defaultChoice;
		mCurrent = mTree[mCurrent]->Next[choice];
	}
	else
		std::cout << "Error: Invalid choice reported" << choice << std::endl; //Maybe make an exception here later?

	//Check for end of tree. Either the next child is one past the size of the tree or the next one leads to a different, unrelated branch
	if (mTree[mCurrent]->Next[0] >= mTree.size() || mTree[mTree[mCurrent]->Next[0]]->Parent != mCurrent)
	{
		sNodeType = DIALOGUE_END;
		return mCurrent;
	}

	switch (mTree[mCurrent]->type) //Check its type of set the state accordingly
	{
	case DIALOGUE_LINE:
		sNodeType = DIALOGUE_LINE;
		break;
	case DIALOGUE_CHOICE:
		sNodeType = DIALOGUE_CHOICE;
		break;
	default:
		std::cout << "How did I break it this time?" << std::endl;
		sNodeType = DIALOGUE_NULL;
		break;
	}

	return mCurrent;
}

//Returns the index of the new current node
int CDialogueTree::Backtrack (void)
{
	if (mTree[mCurrent]->Parent > -1)
		mCurrent = mTree[mCurrent]->Parent;
	
	switch (mTree[mCurrent]->type) //Check its type of set the state accordingly
	{
	case DIALOGUE_LINE:
		sNodeType = DIALOGUE_LINE;
		break;
	case DIALOGUE_CHOICE:
		sNodeType = DIALOGUE_CHOICE;
		break;
	default:
		std::cout << "How did I break it this time?" << std::endl;
		sNodeType = DIALOGUE_NULL;
		break;
	}

	return mCurrent;
}

const char * CDialogueTree::GetLine (void)
{
	if (mTree[mCurrent]->type == DIALOGUE_LINE)
		return mTree[mCurrent]->Lines[0].c_str();
	else
		std::cout << "Error: Calling GetLine when node is not a line type. Use GetChoices() instead" << std::endl;
	
	return nullptr;
}

std::vector<string> CDialogueTree::GetChoices (void)
{
	if (mTree[mCurrent]->type == DIALOGUE_CHOICE)
		return mTree[mCurrent]->Lines;
	std::cout << "Error: Calling GetChoices() when node is a Line type. Call GetLine() instead" << std::endl;
	std::vector<string> empty;
	return empty; //There's gotta be a better way
}

int CDialogueTree::GetFlags (void)
{
	return mTree[mCurrent]->flags;
}

int CDialogueTree::GetLength (void)
{
	return mTree[mCurrent]->length;
}

void CDialogueTree::PrintTree (void)
{
	for (size_t i = 0; i < mTree.size(); i++)
	{
		if (mTree[i]->type == DIALOGUE_LINE)
		{
			DialogueNode *current = mTree[i];

			std::cout << "ID: " << current->ID << " " << "Character: " << current->character << '\n';
			std::cout << "Address: " << current->address << " Flags: " << current->flags << '\n';
			std::cout << "Line: " << current->Lines[0] << '\n';
			std::cout << "Parent: " << current->Parent << " | " << "Next: " << current->Next[0] << '\n';
			std::cout << "Sound: " << current->sound << '\n';
			std::cout << "Motion: " << current->motion << '\n';
		}
		else if (mTree[i]->type == DIALOGUE_CHOICE)
		{
			DialogueNode *current = mTree[i];
			std::cout << "ID: " << current->ID << " " << "Character: " << current->character << '\n';
			std::cout << "Address: " << current->address << " Flags: " << current->flags << '\n';
			std::cout << "Lines:\n";
			for (size_t j = 0; j < current->Lines.size(); j++)
				std::cout << current->Lines[j] << '\n';
			std::cout << "Parent: " << current->Parent << "\nNext:\n";
			for (size_t k = 0; k < current->Next.size(); k++)
				std::cout << current->Next[k] << '\n';
			std::cout << "Sound: " << current->sound << '\n';
			std::cout << "Motion: " << current->motion << '\n';
		}
	}
}

/*
CDialogueTree::GetSound (void)
CDialogueTree::GetMotion (void)
CDialogueTree::GetCharacter (void)
*/
