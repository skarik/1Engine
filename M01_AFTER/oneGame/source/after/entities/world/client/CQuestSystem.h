#ifndef _CQUESTSYSTEM_H_
#define _CQUESTSYSTEM_H_

#include <string>
#include <vector>

#include "core/types/types.h"
#include "core/math/Rect.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"
#include "after/types/Disciplines.h"

class CBitmapFont;

enum QuestStatus : uint8_t
{
	FAIL = 0,
	INPROG = 1,
	COMPELETE = 2,
	PAUSE = 3,
	LIMBO = 4,
};

struct ClassXP
{
	Discipline classtype;
	short xp;
};

struct Loyalties
{
	std::string faction;
	float loyalty;
};


struct Objectives
{
	bool critical;
	bool isCurrent; // is this the current objective they are working on in the quest
	QuestStatus Status;
	
};

struct Quests
{
	short idNum; //The quest Type, this identifies what the quest style of quest it is.
	short instNum; // A reference Identifer, unique across all quests.
	
	//Quest Visual Properties
	std::string name;
	std::string summary;
	std::string type;
	std::string subtype;
	std::string giver;

	//Quest Reward Properties
	short moneyReward;
	short genXP;
	std::vector<ClassXP> classes;
	short objTotal;
	std::vector<Objectives> objective;

	//Flags for progress on a quest (Counts for items that need to be checked and what not).
	std::vector<short> questStats;

	//Quest Status VAriables
	bool isActive; //Waypoint Visibilities
	QuestStatus Status;
	
	//LUA Variables
	std::string luaFile;
	arstring<30>	m_environment;	// Object environment
};


class CQuestSystem : public CGameBehavior
{
	public:
	
	CQuestSystem(void);
	CQuestSystem(bool test);
	~CQuestSystem(void);

	void setAsActive(void);
	void displayInfo(void);
	void missionFailed(void);
	void objectiveFailed(int quest, int objective);
	void objectiveConditions(int quest, int objective);
	void updateObjectives(void);
	int questCount(void);
	void saveQuests(void);
	void loadQuests(void);

	void testFunction(void);

	std::vector<Quests>& getQuests(void);

	void Update(void) override;



private:
	string playername;
	std::vector<Quests> questList;
};

class CQuestSysRenderer : public CRenderableObject , public CGameBehavior
{
public:

	CQuestSysRenderer(CQuestSystem* quest);
	~CQuestSysRenderer();

	bool Render(const char pass) override;
	void Update();

private:
	//The quest system object
	CQuestSystem* questsys;

	
	
	//Rendering objects and settings
	CBitmapFont* fntMenu;
	glMaterial* matfntMenu;
	glMaterial* matMenu;

	Rect* scrollBar;

	float lastBarYPos;
	float currBarYPos;
	double sScroll; //Scroll scale, to slow down the movement
	
};



#endif