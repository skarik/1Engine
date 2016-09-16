#include "CQuestSystem.h"

#include "core/system/io/CBinaryFile.h"
#include "core/settings/CGameSettings.h"
#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
#include "core/system/Screen.h"
#include "core/time/time.h"
#include "core/math/Rect.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/texture/CTexture.h"

CQuestSystem::CQuestSystem(void) : CGameBehavior()
{

	for(int i = 0; i <20; i++)
	{
		Quests testquest;
	
		testquest.name = "testName";
		testquest.giver = "testGiver";
		questList.push_back(testquest);
	}

}


CQuestSystem::CQuestSystem(bool test) : CGameBehavior()
{
		Quests temp;

		temp.idNum = 0001;
		temp.instNum = 1;
		temp.name = "QuestTest";
		temp.summary = "This is a test summary";
		temp.type = "TestType";
		temp.subtype = "TestSubType";
		temp.giver = "Villager Test";
		temp.moneyReward = 100;
		temp.genXP = 50;
		 
		ClassXP tempClass;	
		temp.classes.push_back(tempClass);
		temp.classes[0].classtype = (Discipline) 1;
		temp.classes[0].xp = 100;
		

		temp.objTotal = 1;
			
		Objectives tempObjective;
		temp.objective.push_back(tempObjective);
		temp.objective[0].critical = true;
		temp.objective[0].isCurrent = false;
		temp.objective[0].Status = INPROG;

		temp.isActive = true;
		temp.Status = INPROG;

		questList.push_back(temp);
}


CQuestSystem::~CQuestSystem(void)
{

}

void CQuestSystem::setAsActive(void)
{
	//TODO?: Make a check for if it should be active or not.
	for(uint questnum = 0; questnum < questList.size(); questnum++)
	{
		if(questList[questnum].Status == INPROG)
			questList[questnum].isActive = true;
	}
}

void CQuestSystem::testFunction(void)
{
	std::cout << "This is the quest entries before being saved\n";
	std::cout << (questList[0].idNum) << "\n";
	std::cout << (questList[0].instNum) << "\n";
	std::cout << (questList[0].name) << "\n";
	std::cout << (questList[0].summary) << "\n";
	std::cout << (questList[0].type) << "\n";
	std::cout << (questList[0].subtype) << "\n";
	std::cout << (questList[0].giver) << "\n";
	std::cout << (questList[0].moneyReward) << "\n";
	std::cout << (questList[0].genXP) << "\n";
	std::cout << (questList[0].objTotal) << "\n";
	std::cout << (questList[0].objective[0].critical) << "\n";
	std::cout << (questList[0].objective[0].isCurrent) << "\n";
	std::cout << (questList[0].objective[0].Status) << "\n";

	std::cout << (questList[0].isActive) << "\n";

	std::cout << "Attempting to save Quest data.\n";
	saveQuests();
	
	questList.pop_back();

	std::cout << "Quest saved, Attempting to load the data back.\n";
	loadQuests();

	std::cout << "This is what the data looks like after loading it in.\n";

	std::cout << (questList[0].idNum) << "\n";
	std::cout << (questList[0].instNum) << "\n";
	std::cout << (questList[0].name) << "\n";
	std::cout << (questList[0].summary) << "\n";
	std::cout << (questList[0].type) << "\n";
	std::cout << (questList[0].subtype) << "\n";
	std::cout << (questList[0].giver) << "\n";
	std::cout << (questList[0].moneyReward) << "\n";
	std::cout << (questList[0].genXP) << "\n";
	std::cout << (questList[0].objTotal) << "\n";
	std::cout << (questList[0].objective[0].critical) << "\n";
	std::cout << (questList[0].objective[0].isCurrent) << "\n";
	std::cout << (questList[0].objective[0].Status) << "\n";

	std::cout << (questList[0].isActive) << "\n";

}


//Checks if a mission has met its failed conditionals.
void CQuestSystem::missionFailed()
{
	for(int questNum = 0; questNum < questList.size(); questNum++)
	{
		if(questList[questNum].Status == INPROG)
		{
			for(int objNum = 0; objNum < questList[questNum].objTotal; objNum++)
			{
				//if objective is failed and critical, then whole mission is failed, else leave it be.
				if(questList[questNum].objective[objNum].Status == FAIL && questList[questNum].objective[objNum].critical == true && questList[questNum].Status != FAIL)
				{
					questList[questNum].Status = FAIL;
					questList[questNum].isActive = false;
				}
			}
		}
	}
}


void CQuestSystem::objectiveFailed(int quest, int objective)
{
	//if dependencies dictate that a objective is failed, then set its status as failed.
}

void CQuestSystem::objectiveConditions(int quest, int objective)
{
	//check for changes in the dependencies
}

void CQuestSystem::updateObjectives()
{
	for(int questNum = 0; questNum < questList.size(); questNum++)
	{
		if(questList[questNum].Status == INPROG)
		{
			for(int objNum = 0; objNum < questList[questNum].objTotal; objNum++)
			{
				//Check the status of the objective dependents and change any statuses as needed.
				//TODO: Add an if statment that check if the quest is already failed to skip updating it and what not.
				if(questList[questNum].Status == INPROG)
				{
					objectiveConditions(questNum, objNum);
					objectiveFailed(questNum, objNum);
				}
			}
		}

	}
}

int CQuestSystem::questCount(void)
{
	int count = questList.size();

	return count;
}

std::vector<Quests>& CQuestSystem::getQuests()
{
	return questList;
}

void CQuestSystem::Update(void)
{
	updateObjectives();
	missionFailed();
}

void CQuestSystem::saveQuests(void)
{
	arstring<256> sSaveLocation;
	CBinaryFile questFile;

	
	sprintf(sSaveLocation, "%s/quests", CGameSettings::Active()->MakePlayerSaveDirectory().c_str());
	questFile.Open(sSaveLocation.c_str(), CBinaryFile::IO_WRITE);

	questFile.WriteInt32(questCount());
	for each(Quests q in questList)
	{
			/*
			*	Formatting of the file is as such:
			*	
			*	idNum
			*	instID
			*	name
			*	summary
			*	type
			*	subtype
			*	Giver
			*	moneyReward
			*	genXp
			*	classXpNum
			*	classXpArrayValues
			*	objTotal
			*	objArrayValues
			*	statsNum
			*	StatsArrayValues
			*	isActive
			*	Status
			*	
			*/

		questFile.WriteShort(q.idNum);
		questFile.WriteShort(q.instNum);
		questFile.WriteString(q.name);
		questFile.WriteString(q.summary);
		questFile.WriteString(q.type);
		questFile.WriteString(q.subtype);
		questFile.WriteString(q.giver);
		questFile.WriteShort(q.moneyReward);
		questFile.WriteShort(q.genXP);
		questFile.WriteInt32(q.classes.size());
		
		for each(ClassXP c in q.classes)
		{
			questFile.WriteUInt32(c.classtype);
			questFile.WriteShort(c.xp);
		}

		questFile.WriteShort(q.objTotal);

		for each (Objectives o in q.objective)
		{
			questFile.WriteUChar(o.critical);
			questFile.WriteUChar(o.isCurrent);
			questFile.WriteUChar(o.Status);
		}


		questFile.WriteInt32(q.questStats.size());

		for each(short s in q.questStats)
		{

			questFile.WriteShort(s);

		}

		questFile.WriteUChar(q.isActive);
		questFile.WriteUChar(q.Status);
	}

	questFile.Close();

	
}

void CQuestSystem::loadQuests(void)
{
	arstring<256> sSaveLocation;
	CBinaryFile questFile;
	int questNum, classXPCount, objCount, statCount;

	
	sprintf(sSaveLocation, "%s/quests", CGameSettings::Active()->MakePlayerSaveDirectory().c_str());
	questFile.Open(sSaveLocation.c_str(), CBinaryFile::IO_READ);

	/*
			*	Formatting of the file is as such:
			*	
			*	idNum
			*	instID
			*	name
			*	summary
			*	type
			*	subtype
			*	Giver
			*	moneyReward
			*	genXp
			*	classXpNum
			*	classXpArrayValues
			*	objTotal
			*	objArrayValues
			*	statsNum
			*	StatsArrayValues
			*	isActive
			*	Status
			*	
			*/

	questNum = questFile.ReadInt32();

	

	for(int i = 0; i < questNum; i++)
	{
		Quests newQuest;

		newQuest.idNum = questFile.ReadShort();
		newQuest.instNum = questFile.ReadShort();
		newQuest.name = questFile.ReadString();
		newQuest.summary = questFile.ReadString();
		newQuest.type = questFile.ReadString();
		newQuest.subtype = questFile.ReadString();
		newQuest.giver = questFile.ReadString();
		newQuest.moneyReward = questFile.ReadShort();
		newQuest.genXP = questFile.ReadShort();
		
		classXPCount = questFile.ReadInt32();
		
		for(int j = 0; j < classXPCount; j++)
		{
			ClassXP xp;

			xp.classtype = ((Discipline)questFile.ReadInt32());
			xp.xp = questFile.ReadShort();

			newQuest.classes.push_back(xp);
		}

		objCount = questFile.ReadShort();

		newQuest.objTotal = objCount;

		for(int k = 0; k < objCount; k++)
		{
			Objectives newObj;

			newObj.critical = questFile.ReadUChar();
			newObj.isCurrent = questFile.ReadUChar();
			newObj.Status = (QuestStatus)questFile.ReadUChar();

			newQuest.objective.push_back(newObj);
		}


		statCount = questFile.ReadInt32();


		for(int l = 0; l < statCount; l++)
		{
			
			newQuest.questStats.push_back(questFile.ReadShort());
		}

		newQuest.isActive = questFile.ReadUChar();
		newQuest.Status = ((QuestStatus)questFile.ReadUChar());

		questList.push_back(newQuest);
	}

	questFile.Close();


}


CQuestSysRenderer::CQuestSysRenderer(CQuestSystem* quest) :CRenderableObject(), CGameBehavior()
{
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	fntMenu	= new CBitmapFont ( "monofonto.ttf", 18, FW_BOLD );
	matfntMenu = new glMaterial;
	matfntMenu->m_diffuse = Color( 1.0f,1,1 );
	matfntMenu->setTexture( 0, fntMenu );
	matfntMenu->passinfo.push_back( glPass() );
	matfntMenu->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matfntMenu->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matMenu = new glMaterial;
	matMenu->m_diffuse = Color( 0,0,0 );
	matMenu->setTexture( 0, new CTexture("null") );
	matMenu->passinfo.push_back( glPass() );
	matMenu->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matMenu->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	currBarYPos = .15F;
	lastBarYPos = .15F;

	SetMaterial( matMenu );


	scrollBar = new Rect(.45F, .15F, .05F, .15F);

	transform.position.z = -30;

	visible = false;

	questsys = quest;
}

CQuestSysRenderer::~CQuestSysRenderer()
{

}

bool CQuestSysRenderer::Render(const char pass)
{
	int value = std::max<int>(0, questsys->questCount() - 10) * (currBarYPos - .15/.7);

	if(pass != 0)
	{
		return true;
	}

	GL_ACCESS;
	GLd_ACCESS;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	matMenu->m_diffuse = Color(0, 0, 0, 1);
		matMenu->bindPass(0);

	GLd.DrawRectangleA(scrollBar->pos.x, scrollBar->pos.y, scrollBar->size.x, scrollBar->size.y);

	matMenu->m_diffuse = Color(1, 1, 1, 0.5);
		matMenu->bindPass(0);
			GLd.DrawRectangleA( 0,0,2,2 );
			matfntMenu->bindPass(0);
			
			for(int i = std::max<int>(0, questsys->questCount() - 10) * (currBarYPos - .15/.7); i < questsys->questCount(); i++)
			{
				GLd.DrawAutoText(.06, .15 + .07 * i - value, questsys->getQuests()[i].name.c_str());
			}


	GL.endOrtho();

	return true;
}

void CQuestSysRenderer::Update()
{
	Vector2d cursor_position( CInput::MouseX() / (ftype)Screen::Info.width, CInput::MouseY() / (ftype)Screen::Info.height );

	if(Input::Mouse(Input::MBLeft) && scrollBar->Contains(cursor_position))
	{	

		currBarYPos = cursor_position.y;

		scrollBar->pos.y = currBarYPos - scrollBar->size.y/2;

		if(scrollBar->pos.y + scrollBar->size.y > 0.85f)
		{
			scrollBar->pos.y = .85f - scrollBar->size.y;
		}

		if(scrollBar->pos.y < 0.15f)
		{
			scrollBar->pos.y = .15f;
		}


		lastBarYPos = scrollBar->pos.y;
	}
}