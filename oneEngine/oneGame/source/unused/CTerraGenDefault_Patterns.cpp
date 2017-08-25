
// ======== CTerraGenDefault_Patterns =======
// This contains the definition of parts of the region/pattern generation of the terrain.
// 

#include "CTerraGenDefault.h"

#include "CRegionManager.h"
#include "CTownManager.h"

using std::cout;
using std::endl;
using std::max;
using std::min;

void CTerraGenDefault::TerraGenPatternPass( CBoob* pBoob, const RangeVector & position )
{
	try
	{
		// Generate regions first.
		for ( char i = -5; i <= 5; ++i ) {
			for ( char j = -5; j <= 5; ++j ) {
				regionManager->Generate( RangeVector( position.x+i,position.y+j,0 ) );
			}
		}
	}
	catch ( std::exception any_err ) // don't do this
	{
		cout << "ERROR IN REGIONS: " << any_err.what() << endl;
	};
	
	try
	{
		// Generate towns after.
		for ( char i = -5; i <= 5; ++i ) {
			for ( char j = -5; j <= 5; ++j ) {
				townManager->Generate( RangeVector( position.x+i,position.y+j,0 ) );
			}
		}
	}
	catch ( std::exception any_err ) // don't do this
	{
		cout << "ERROR IN TOWNS: " << any_err.what() << endl;
	};

	// Now, create towns
	townManager->CreateFirstPass( pBoob,position );
	
}

