#ifndef _C_CLOUD_AI_H_
#define _C_CLOUD_AI_H_

#include "CCloudEnemy.h"
#include "CPersonality.h"

class CCloudAI: public CGameObject
{
public:
	//Default constructor because it'll be useful somewhere
	CCloudAI();
	//Host-only constructor. Will randomly pick a personality
	CCloudAI(CCloudEnemy *host);
	//Full constructor
	CCloudAI(CCloudEnemy *host, CPersonality *personality);

	//Destructor
	~CCloudAI();

	//Only use this function if the default constructor was used for some reason
	void Initialize(CCloudEnemy *host, CPersonality *personality);
	//Duh. Change this AI's personality
	void ChangePersonality(CPersonality *personality);

	//Update step overrides
	void Update() override;
	void LateUpdate() override;
	void PostUpdate() override;

private:
	CCloudEnemy *pHost;
	CPersonality *pPersonality;
};

#endif
