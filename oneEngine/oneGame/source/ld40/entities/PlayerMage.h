#ifndef M04_PLAYER_MAGE_H_
#define M04_PLAYER_MAGE_H_

#include "PlayerPlatformerBase.h"

class CInputControl;
class COrthoCamera;
class CLight;
class CRigidbody;

namespace M04
{
	//		PlayerMage
	// Basic platformer player.
	class PlayerMage : public PlayerPlatformerBase
	{
	public:
		explicit		PlayerMage ( void );
						~PlayerMage ( void );

		void			Update ( void ) override;
		void			FixedUpdate ( void ) override;
		void			PostFixedUpdate ( void ) override;

	private:
		uint32_t		anim_idlePose;
		uint32_t		anim_idleReady;
		uint32_t		anim_fall;

		Real			afx_idleTime;

	protected:

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::PlayerMage);
		DEFINE_DISPLAY(DISPLAY_2D_SPRITE,"sprites/ld40/mage2.gal");
		DEFINE_VALUE(position,Vector3d,FIELD_POSITION);
		END_OBJECT_DESC();
	};
};

#endif//M04_PLAYER_MAGE_H_