
#include "TestEntityBasic.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(test_entity_basic,M04::TestEntityBasic);

TestEntityBasic::TestEntityBasic ( void )
	: CGameBehavior(), Engine2D::SpriteContainer( &position )
{
	;
}

TestEntityBasic::~TestEntityBasic ( void )
{

}

void TestEntityBasic::Update ( void )
{
	; // Nothing.
}