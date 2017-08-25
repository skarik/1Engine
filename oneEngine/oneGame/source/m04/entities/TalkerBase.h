#ifndef _M04_TALKER_BASE_H_
#define _M04_TALKER_BASE_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"

namespace M04
{
	class TalkerBase : public CGameBehavior
	{
	protected:
		explicit TalkerBase ( void )
			: CGameBehavior()
		{
			;
		}

	public:
		~TalkerBase ( void )
		{
			;
		}

	};
}

#endif//_M04_TALKER_BASE_H_