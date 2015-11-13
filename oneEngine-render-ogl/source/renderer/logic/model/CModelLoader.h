#ifndef _C_MODEL_LOADER_H_
#define _C_MODEL_LOADER_H_

#include "core/types/types.h"
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>

struct ModelHeader
{
	uint32_t meshnum;
	uint32_t uniquemeshnum;
	uint32_t animated;
	uint32_t framenumber;
	uint32_t materialnum;
	uint32_t bonenum;
	uint32_t splinenum;
	uint32_t m_version;
	uint32_t cameranum;
	uint32_t miscnum;
	uint32_t morphnum;
	uint32_t morphmesh;
	uint64_t datetime;
};

#endif