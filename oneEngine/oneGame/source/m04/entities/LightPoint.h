#ifndef _M04_LIGHT_POINT_H_
#define _M04_LIGHT_POINT_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

class RrLight;

namespace M04
{
	//		LightPoint
	// Basic engine-serialized light object
	class LightPoint : public CGameBehavior
	{
	public:
		explicit		LightPoint ( void );
		~LightPoint ( void );

		void			Update ( void ) override;
	protected:
		Vector3f position;
		float range;
		float power;
		Color color;

		RrLight*	m_light;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::LightPoint);
			DEFINE_DISPLAY(DISPLAY_LIGHT,"sprites/editor/lightbulb.png");
			DEFINE_VALUE(position,Vector3f,FIELD_POSITION);
			DEFINE_VALUE(range,float,FIELD_DEFAULT);
			DEFINE_VALUE(power,float,FIELD_DEFAULT);
			DEFINE_VALUE(color,Color,FIELD_COLOR);	
		END_OBJECT_DESC();
	};
}

#endif//_M04_LIGHT_POINT_H_