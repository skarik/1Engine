#ifndef _M04_LIGHT_POINT_H_
#define _M04_LIGHT_POINT_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

namespace M04
{
	//		TestEntityBasic
	// Testing basic 2D functionality
	class LightPoint : public CGameBehavior//, public Engine2D::SpriteContainer
	{
	public:
		explicit		LightPoint ( void );
		~LightPoint ( void );

		void			Update ( void ) override;
	protected:
		Vector3d position;
		float range;
		float intensity;
		Color color;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::LightPoint);
			DEFINE_DISPLAY(DISPLAY_LIGHT,"sprites/editor/lightbulb.png");
			DEFINE_VALUE(position,Vector3d,FIELD_POSITION);
			DEFINE_VALUE(range,float,FIELD_DEFAULT);	
			DEFINE_VALUE(intensity,float,FIELD_DEFAULT);
			DEFINE_VALUE(color,Color,FIELD_COLOR);	
		END_OBJECT_DESC();
	};
}

// Create editor object
LINK_OBJECT_TO_CLASS(light_point,M04::LightPoint);

#endif//_M04_LIGHT_POINT_H_