
#ifndef _S_HITBOX_H_
#define _S_HITBOX_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/math/Vector3d.h"

struct sHitbox
{
	uchar			indexLink;
	uchar			parentIndex;
	arstring<64>	name;
	Vector3d		center;
	Vector3d		extents;
};


#endif//_S_HITBOX_H_