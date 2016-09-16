
#ifndef _PATTERN_DESERT_RUINS_H_
#define _PATTERN_DESERT_RUINS_H_

#include "../IPatternGeneration.h"

namespace Terrain
{
	class PatternDesertRuins : public IPatternGeneration
	{
	public:
		void Generate ( void ) override;
	};
};

#endif//_PATTERN_DESERT_RUINS_H_