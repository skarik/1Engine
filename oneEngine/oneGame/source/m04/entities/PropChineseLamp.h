#ifndef _M04_PROP_CHINESE_LAMP_H_
#define _M04_PROP_CHINESE_LAMP_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

class RrLight;

namespace M04
{
	//		PropChineseLamp
	// Chinese lamp object. All cute and shit.
	class PropChineseLamp : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit		PropChineseLamp ( void );
						~PropChineseLamp ( void );

		void			Update ( void ) override;
	protected:
		Vector3f position;
		float height;
		Color color;

		RrLight*	m_light;
		float m_angle;

		// Expose values to the editor and serializer
		BEGIN_OBJECT_DESC(M04::PropChineseLamp);
		DEFINE_DISPLAY(DISPLAY_LIGHT,"sprites/props/chinese_lamp.gal");
		DEFINE_VALUE(position,Vector3f,FIELD_POSITION);
		DEFINE_VALUE(height,float,FIELD_DEFAULT);
		DEFINE_VALUE(color,Color,FIELD_COLOR);	
		END_OBJECT_DESC();
	};
}

#endif//_M04_PROP_CHINESE_LAMP_H_