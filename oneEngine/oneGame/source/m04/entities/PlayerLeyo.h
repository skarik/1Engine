#ifndef _M04_PLAYER_LEYO_H_
#define _M04_PLAYER_LEYO_H_

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
	class UILuvPpl;

	//		PlayerLeyo
	// Basic main character nonsense.
	class PlayerLeyo : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit			PlayerLeyo ( void );
							~PlayerLeyo ( void );

		void				Update ( void ) override;
		void				PostFixedUpdate ( void ) override;

	public:
		static PlayerLeyo*
						active;

	public:
		// Current state:

		Vector3f		flipstate;
		
		Vector3f		position;
		Vector3f		velocity;

		int				camera_mode;
		float			camera_lerp_mode;
		float			camera_stuck_time;
		Vector3f		camera_position;
		Vector3f		camera_lockposition;

	protected:

		void				CameraUpdate ( void );


	protected:

		// Components:
		
		CInputControl*	input;
		COrthoCamera*	camera;
		RrLight*		light;
		CRigidbody*		bod;
		UILuvPpl*		ui;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::PlayerLeyo);
		DEFINE_DISPLAY(DISPLAY_2D_SPRITE,"sprites/leo.gal");
		DEFINE_VALUE(position,Vector3f,FIELD_POSITION);
		END_OBJECT_DESC();
	};
}

#endif//_M04_PLAYER_LEYO_H_