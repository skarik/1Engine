#ifndef _C_WAYPOINT_H_
#define _C_WAYPOINT_H_

#include "CPointBase.h"
#include <vector>

class RrCModel;

class CWaypoint: public CPointBase
{
	ClassName ("Waypoint");

public:
	ENGCOM_API CWaypoint (void);
	ENGCOM_API ~CWaypoint (void);

	ENGCOM_API void Update (void);

public:
	enum eWaypointType
	{
		PLAIN = 0,
		QUEST,
		SIG_ITEM,
		SIG_PERSON,
		OTHER_PLAYER,
		ENTITY_TYPE,
		KING_HILL,
		CAPTURE_FLAG
	};
	eWaypointType type;

	Real distance;
	bool in_range;

	RrCModel* entity;

public:
	ENGCOM_API static std::vector<CWaypoint*> WaypointList;
	
};

#endif