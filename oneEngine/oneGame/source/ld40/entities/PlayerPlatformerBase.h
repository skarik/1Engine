#ifndef M04_PLAYER_PLATFORMER_BASE_H_
#define M04_PLAYER_PLATFORMER_BASE_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/AnimationContainer.h"

class CInputControl;
class COrthoCamera;
class RrLight;
class CRigidbody;

namespace M04
{
	class CameraControllerPlatformer;
	class CharacterControllerPlatformer;

	//		PlayerPlatformerBase
	// Basic platformer player.
	class PlayerPlatformerBase : public CGameBehavior, public Engine2D::AnimationContainer
	{
	public:
		explicit		PlayerPlatformerBase ( void );
						~PlayerPlatformerBase ( void );

		void			OnCreate ( void ) override;

		void			Update ( void ) override;
		void			FixedUpdate ( void ) override;
		void			PostFixedUpdate ( void ) override;

	public:
		// Current state:

		Vector3f flipstate;

		Vector3f position;
		Vector3f velocity;

		Vector3f display_position;

	protected:

		// Components:

		CameraControllerPlatformer*		camera;
		CharacterControllerPlatformer*	motion;
		CInputControl*					input;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::PlayerPlatformerBase);
		DEFINE_DISPLAY(DISPLAY_BOX,"1");
		DEFINE_VALUE(position,Vector3f,FIELD_POSITION);
		END_OBJECT_DESC();
	};
};

#endif//M04_PLAYER_PLATFORMER_BASE_H_