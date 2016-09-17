#include "CCloudAI.h"

CCloudAI::CCloudAI () { }

CCloudAI::CCloudAI(CCloudEnemy *host): pHost(host)
{
	//Throw an exception because someone is being stupid
	if (pHost == NULL)
		throw "CCouldAI Error: NULL host (0)";
	
	//Randomly pick a personality and use it
}

CCloudAI::CCloudAI (CCloudEnemy *host, CPersonality *personality): pHost(host), pPersonality(personality)
{
	//Throw an exception because someone is a simp
	if (pHost == NULL)
		throw "CCloudAI Error: NULL host (1)";
	
	if (pPersonality == NULL)
	{
		//Randomly pick a personality and use it because the twat who called this passed in null
	}
}

CCloudAI::~CCloudAI ()
{
	if (pHost != NULL)
		delete pHost;

	if (pPersonality != NULL)
		delete pPersonality;
}

void CCloudAI::Initialize (CCloudEnemy *host, CPersonality *personality)
{
	if (host == NULL)
		throw "CCloudAI Error: NULL host (2)";

	if (personality == NULL)
	{
		//Randomly pick a personality and use it because the moron who called this passed in null
	}
}