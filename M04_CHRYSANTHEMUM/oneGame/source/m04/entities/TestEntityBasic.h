#ifndef _TEST_ENTITY_BASIC_H_
#define _TEST_ENTITY_BASIC_H_

#include "core/math/Math3d.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine2d/interface/SpriteContainer.h"

namespace M04
{
	//		TestEntityBasic
	// Testing basic 2D functionality
	class TestEntityBasic : public CGameBehavior, public Engine2D::SpriteContainer
	{
	public:
		explicit		TestEntityBasic ( void );
						~TestEntityBasic ( void );

		void			Update ( void ) override;
	protected:
		Vector3d position;
	};
}

// Expose values to the editor and serializer
BEGIN_OBJECT_DESC(M04::TestEntityBasic);
	DEFINE_DISPLAY(DISPLAY_2D_SPRITE,"sprites/test0");
	DEFINE_VALUE(position,FIELD_POSITION);				// This will allow the object to be moved properly in the editor.
END_OBJECT_DESC();

// Create editor object
LINK_OBJECT_TO_CLASS(test_entity_basic,M04::TestEntityBasic);

#endif//_TEST_ENTITY_BASIC_H_