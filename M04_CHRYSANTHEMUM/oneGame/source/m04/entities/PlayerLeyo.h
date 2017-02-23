#ifndef _M04_PLAYER_LEYO_H_
#define _M04_PLAYER_LEYO_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

class CInputControl;
class COrthoCamera;
class CLight;

namespace M04
{
	//		PlayerLeyo
	// Basic main character nonsense.
	class PlayerLeyo : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit		PlayerLeyo ( void );
		~PlayerLeyo ( void );

		void			Update ( void ) override;
	protected:
		// Current state:
		
		Vector3d position;
		Vector3d velocity;

		// Components:
		
		CInputControl* input;
		COrthoCamera* camera;
		CLight* light;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::PlayerLeyo);
		DEFINE_DISPLAY(DISPLAY_2D_SPRITE,"sprites/leo.gal");
		DEFINE_VALUE(position,Vector3d,FIELD_POSITION);
		END_OBJECT_DESC();
	};
}

// Create editor object
LINK_OBJECT_TO_CLASS(player_leyo,M04::PlayerLeyo);

#endif//_M04_PLAYER_LEYO_H_