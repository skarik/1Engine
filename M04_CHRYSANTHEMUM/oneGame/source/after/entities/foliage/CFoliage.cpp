

#include "CFoliage.h"
//#include "unused/CBoob.h"

// Include debugger
#include "renderer/debug/CDebugDrawer.h"

// Constructor and Destructor
CFoliage::CFoliage ( void )
	: CActor()
{


}
CFoliage::~CFoliage ( void )
{
	
}

void CFoliage::RemoveFromTerrain ( void )
{
	// Find self in terrain and remove
	/*CBoob::FoliageIterator it = find( pBoob->v_foliage.begin(), pBoob->v_foliage.end(), this );
	if ( it != pBoob->v_foliage.end() )
		pBoob->v_foliage.erase( it );
	else
		cout << "Warning: Unable to find foliage in associated terrain!" << endl;*/
}

// == Save Data Common ==
/*void CFoliage::SaveData ( ostream& svout )
{
	svout << transform.position.x << endl;
	svout << transform.position.y << endl;
	svout << transform.position.z << endl;
}
// == Load Data Common ==
void CFoliage::LoadData ( istream& ldin )
{
	ldin >> transform.position.x;
	ldin >> transform.position.y;
	ldin >> transform.position.z;
}*/