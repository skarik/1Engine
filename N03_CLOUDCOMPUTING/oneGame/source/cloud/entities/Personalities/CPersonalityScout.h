#ifndef _C_PERSONALITY_SCOUT_H_
#define _C_PERSONALITY_SCOUT_H_

#include "../CPersonality.h"

//This personality type tries to spread out and keep vision over the area
class CPersonalityScout : public CPersonality
{
public:
	CPersonalityScout () {}
	CPersonalityScout (CCloudEnemy *host);
	~CPersonalityScout();

	void Execute(Rotator &turn, Vector3d &acceleration, int &flags) override;

private:
	//Keep track of the starting position
	Vector3d mStart;
	//How far away the scout will travel from its start point
	Real mDist;
	//Keep track of if the scout is close enough to destination
	bool mIsAtDest;
	//Random direction to travel in
	Vector3d mDirection;

	Real DistanceFromStart(void);
};

#endif