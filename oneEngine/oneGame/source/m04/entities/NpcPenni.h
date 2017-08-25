#ifndef _M04_NPC_PENNI_H_
#define _M04_NPC_PENNI_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "m04/entities/NpcBase.h"

class CRigidbody;

namespace M04
{
	class TalkerBox;

	//		NpcPenni
	// Basic side character nonsense.
	class NpcPenni : public NpcBase
	{
	public:
		explicit		NpcPenni ( void );
		~NpcPenni ( void );

		void			Update ( void ) override;
	protected:
		TalkerBox*		talker;
		CRigidbody*		bod;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::NpcPenni);
		DEFINE_DISPLAY(DISPLAY_2D_SPRITE,"sprites/penni.gal");
		DEFINE_VALUE(position,Vector3d,FIELD_POSITION);
		END_OBJECT_DESC();
	};
}

#endif//_M04_NPC_PENNI_H_