
#include "CTownGenerationInterface.h"
#include "CTerraGenDefault.h"

bool CTownGenerationInterface::Chance ( const ftype chance, const int seed )
{
	mseed = 6287*mseed + 997;
	//ftype x,y,z;
	/*x = 213.2132f*((seed+mseed)%53) + ((seed^(0x1f23a))&0x0FF) + townCenterPosition.x*(2.322321f+seed);
	y = 123.2321f*seed + ((mseed^(0x1f23a))%43) + townCenterPosition.y*(4.235235f+seed);
	z = seed*12.1f+((genTown->buildings.size()+mseed)%42)*526.1f;*/
	ftype value = generator->noise->Unnormalize(
		   generator->noise_hf->Get3D(
			213.2132f*((seed+mseed)%53) + ((seed^(0x1f23a))&0x0FF) + townCenterPosition.x*(2.322321f+seed),
			123.2321f*seed + ((mseed^(0x1f23a))%43) + townCenterPosition.y*(4.235235f+seed),
			seed*12.1f+((genTown->buildings.size()+mseed)%42)*526.1f
			)
		   )+0.5f;
	//printf( "%lf %lf %lf %lf\n", (double)value, (double)x,(double)y,(double)z );
	//cout << value << " " << x << " " << y << " " << z << endl;
	if ( value < chance ) {
		return true;
	}
	return false;
}
// Gets a random number from -0.5 to 0.5
ftype CTownGenerationInterface::RandomNumber ( void )
{
	mseed = 6287*mseed + 997;
	return ( generator->noise->Unnormalize(
			  generator->noise_hhf->Get3D(
				(mseed%97)*areaPosition.x*324.2121f+areaPosition.y+((mseed^0x1241)&0xFF),
				(mseed%97)*12.23213f + (mseed&0xFF)*areaPosition.z*896.7679f+((mseed^0xfa221)&0xFF),
				(mseed%97)*526.1278f + 23.2123f + areaPosition.x*42.1512f
				)
			)
		);
}