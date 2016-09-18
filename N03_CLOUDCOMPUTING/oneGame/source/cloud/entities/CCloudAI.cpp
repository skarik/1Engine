#include "CCloudAI.h"

CCloudAI::CCloudAI () { }

CCloudAI::CCloudAI(CCloudEnemy *host): pHost(host)
{
	//Throw an exception because someone is being stupid
	if (pHost == NULL)
		throw "CCouldAI Error: NULL host (0)";
	
	//Randomly pick a personality and use it
	pPersonality = pfac->MakePersonality(host, -1);
}

CCloudAI::CCloudAI (CCloudEnemy *host, int type): pHost(host)
{
	//Throw an exception because someone is a simp
	if (pHost == NULL)
		throw "CCloudAI Error: NULL host (1)";
	
	//Make the personality
	pPersonality = pfac->MakePersonality(host, type);
}

CCloudAI::~CCloudAI ()
{
	//Don't delete pHost because something else does, unsurprisingly

	if (pPersonality != nullptr)
		delete pPersonality;
}

void CCloudAI::Initialize (CCloudEnemy *host, int type)
{
	if (host == NULL)
		throw "CCloudAI Error: NULL host (2)";

	pHost = host;
	pPersonality = pfac->MakePersonality(host, type);
}

void CCloudAI::ChangePersonality(CCloudEnemy *host, int type)
{
	if (pPersonality)
		delete pPersonality;

	pPersonality = pfac->MakePersonality(host, type);
}

void CCloudAI::Update() 
{
	pPersonality->Execute(mRot, mAcc, mFlags);

	//pHost->SetTurnInput(mTurn);
	pHost->SetRotation(mRot);
	pHost->SetDirInput(mAcc);
	pHost->SetVAxes(mFlags);
}