#ifndef _M04_NPC_BASE_H_
#define _M04_NPC_BASE_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

class CEmulatedInputControl;

namespace M04
{
	//		NpcBase
	// Basic engine-serialized light object
	class NpcBase : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit		NpcBase ( void );
		~NpcBase ( void );

		void			Update ( void ) override;
	protected:
		// Current state:

		Vector3d position;
		Vector3d velocity;

		// Components:

		CEmulatedInputControl* input;
	};
}

#endif//_M04_NPC_BASE_H_