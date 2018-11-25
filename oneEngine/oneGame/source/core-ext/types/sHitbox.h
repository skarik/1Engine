
#ifndef _S_HITBOX_H_
#define _S_HITBOX_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/math/Vector3.h"

struct sHitbox
{
	uchar			indexLink;
	uchar			parentIndex;
	arstring<64>	name;
	Vector3f		center;
	Vector3f		extents;
};


#endif//_S_HITBOX_H_