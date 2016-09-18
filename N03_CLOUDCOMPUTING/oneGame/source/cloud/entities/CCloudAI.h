#ifndef _C_CLOUD_AI_H_
#define _C_CLOUD_AI_H_

#include "CCloudEnemy.h"
#include "CPersonality.h"
#include "CPersonalityFactory.h"

class CCloudAI: public CGameObject
{
public:
	//Default constructor because it'll be useful somewhere
	CCloudAI(); //Be sure to call Initialize() with proper arguments before trying to use the AI
	//Host-only constructor. Will randomly pick a personality
	CCloudAI(CCloudEnemy *host);
	//Full constructor
	CCloudAI(CCloudEnemy *host, int type);

	//Destructor
	~CCloudAI();

	//Only use this function if the default constructor was used for some reason
	void Initialize(CCloudEnemy *host, int type);
	//Duh. Change this AI's personality
	void ChangePersonality(CCloudEnemy *host, int type);

	//Update step overrides
	void Update() override;
	void LateUpdate() override {}
	void PostUpdate() override {}

private:
	CCloudEnemy *pHost;
	CPersonality *pPersonality;

	Rotator mRot;
	Vector3d mTurn;
	Vector3d mAcc;
	int mFlags;
};

#endif
